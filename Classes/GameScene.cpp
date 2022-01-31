//
//  GameScene.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 19/07/15.
//
//

#include "GameScene.h"

#include "2d/CCLabel.h"
#include "2d/CCScene.h"
#include "2d/CCTransition.h"
#include "2d/CCRenderTexture.h"
#include "cocosbuilder/CocosBuilder.h"
#include "chipmunk/chipmunk.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCDirector.h"

#include "Utils/PLAudioEngine.h"
#include "Utils/AppUtils.h"
#include "Utils/GuiUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/format.h"
#include "Utils/RateScheduler.h"
#include "UI/CCBAnimationRunner.h"
#include "UI/UIProgressHUD.h"
#include "Resources/Resources.h"
#include "ChipmunkDebugNode.h"
#include "GameCenter.h"
#include "Analytics.h"
#include "FirebaseAnalytics.h"
#include "Advertise.h"
#include "RateDialog.h"

#include "SegmentMaker.h"
#include "Game/GameManager.h"
#include "SecondChanceScheduler.h"
#include "MainMenuScene.h"
#include "AppSettings.h"
#include "SecondChanceDialog.h"
#include "PopupsCounter.h"
#include "PauseDialog.h"
#include "AchievementsTracker.h"
#include "BulletNode.h"
#include "Resources/ConfigKeys.h"

USING_NS_CC;
using namespace cocosbuilder;

static int gamesPlayed = 0;
static int lastPlayedLevel = -1;
static float screenshotScale = 1.0f;
cocos2d::Color3B GameScene::GRAY_COLOR = Color3B(202, 202, 202);
cocos2d::Color3B GameScene::SHADOW_GRAY_COLOR = Color3B(182, 182, 182);

Scene *GameScene::createScene() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->unregisterNodeLoader("GameScene");
    nodeLoaderLibrary->registerNodeLoader("GameScene", GameSceneLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    GameScene *rootNode = dynamic_cast<GameScene *>(ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Game::SCENE)));
    ccbReader->release();
    rootNode->forScreenshot = false;
    rootNode->prepareLevel(GameManager::getInstance()->getCurrentLevel(), false);
    rootNode->setTag(GAME_NODE_TAG);
    
    auto scene = Scene::create();
    scene->addChild(rootNode);
    return scene;
}

GameScene *GameScene::createSceneForScreenshot(const cocos2d::Size &size, float scale) {
    screenshotScale = scale;
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->unregisterNodeLoader("GameScene");
    nodeLoaderLibrary->registerNodeLoader("GameScene", GameSceneLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    GameScene *rootNode = dynamic_cast<GameScene *>(ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Game::SCENE), nullptr, size));
    ccbReader->release();
    rootNode->setTag(GAME_NODE_TAG);
    rootNode->forScreenshot = true;
    rootNode->prepareLevel(GameManager::getInstance()->getCurrentLevel(), false);
    
    return rootNode;
}

GameScene::GameScene()
: physicsHelper(nullptr)
, centreMarker(nullptr)
, gameplayNode(nullptr)
, levelNumberLabel(nullptr)
, timeoutLabel(nullptr)
, ringsControlHintLabel(nullptr)
, startHintLabel(nullptr) {
    timeoutFormat = LocalizedString(R::Strings::Game::TIMEOUT_FORMAT);
    
    GuiUtils::setupEscapePressHandler(this, [this]() {
        showPauseDialog();
    });
    
    EventListenerCustom *backgroundListener = EventListenerCustom::create("ResignActive", [this](EventCustom *) {
        showPauseDialog();
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(backgroundListener, this);
}

GameScene::~GameScene() {
    CC_SAFE_RELEASE(physicsHelper);
    CC_SAFE_RELEASE(centreMarker);
    CC_SAFE_RELEASE(gameplayNode);
    CC_SAFE_RELEASE(levelNumberLabel);
    CC_SAFE_RELEASE(timeoutLabel);
    CC_SAFE_RELEASE(ringsControlHintLabel);
    CC_SAFE_RELEASE(startHintLabel);
}
    
bool GameScene::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    ASSIGN_VARIABLE("CentreMarker", centreMarker, Node);
    ASSIGN_VARIABLE("GameplayNode", gameplayNode, Node);
    ASSIGN_VARIABLE("NumberLabel", levelNumberLabel, Label);
    ASSIGN_VARIABLE("TimeoutLabel", timeoutLabel, Label);
    ASSIGN_VARIABLE("RingsControlHint", ringsControlHintLabel, Label);
    ASSIGN_VARIABLE("StartLevelHint", startHintLabel, Node);
    return false;
}

void GameScene::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
    GuiUtils::enableCascadeOpacityRecursively(startHintLabel, true);
    ringsControlHintLabel->setString(LocalizedString(R::Strings::Game::RINGS_CONTROL_HINT));
    for (auto child: startHintLabel->getChildren()) {
        Label *label = dynamic_cast<Label *>(child);
        if (label) {
            label->setString(LocalizedString(R::Strings::Game::START_LEVEL_HINT));
            const float maxWidth = getContentSize().width * 0.9f;
            if (label->getBoundingBox().size.width > maxWidth) {
                label->setScale(maxWidth / label->getBoundingBox().size.width);
            }
        }
    }
    if (ringsControlHintLabel->getContentSize().width > getContentSize().width) {
        ringsControlHintLabel->setScale(getContentSize().width * 0.95f / ringsControlHintLabel->getContentSize().width);
    }
    
    const float height = getContentSize().height;
    if (height < 450) {
        gameScale = height / 520;
    } else if (height < 500) {
        gameScale = height / 500;
    } else {
        gameScale = 1.0f;
    }
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch *touch, Event *event) {
        if (gameType == GameType::RINGS_CONGROL && (gameState == GameState::PREPARE || gameState == GameState::GAME)) {
            touchOrbits();
            return true;
            
        } else if (gameState == GameState::GAME) {
            switch (gameType) {
                case GameType::BULLETS_CONTROL:
                    touchOrbits();
                    sendBullet();
                    break;
                case GameType::RINGS_CONGROL:
                    break;
            }
            return true;
        }
        return false;
    };
    listener->onTouchEnded = [this](Touch *touch, Event *event) {
        if (gameType == GameType::RINGS_CONGROL) {
            touchOrbits();
        }
    };
    listener->onTouchCancelled = listener->onTouchEnded;
    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameScene::onEnter() {
    Node::onEnter();
    scheduleUpdate();
}

void GameScene::onExit() {
    unscheduleUpdate();
    Node::onExit();
}

void GameScene::update(float dt) {
    dt = std::min(dt, 0.04f);
    scaledDt = scaledDt * 0.8f + dt * 0.2f;
    
    if (physicsHelper) {
        physicsHelper->update(scaledDt, gameState == GameState::GAME);
    }
    
    if (useTimeout && gameState == GameState::GAME) {
        timeout -= scaledDt;
        if (timeout < 0) {
            preGameOver(nullptr);
            timeout = 0.0f;
        }
        updateTimeoutLabel();
    }
    
    if (gameState == GameState::GAME && bulletsSchedule.interval > FLT_EPSILON) {
        bulletsSchedule.passedTime += scaledDt;
        if (bulletsSchedule.passedTime >= bulletsSchedule.interval) {
            sendBullet();
            bulletsSchedule.passedTime -= bulletsSchedule.interval;
        }
    }
}

#pragma mark -
#pragma mark Level Startup

void GameScene::prepareLevel(const LevelDefinition &level, bool animateAppear) {
    currentLevelNumber = level.number;
    startHintLabel->setVisible(level.number == 1);
    
    gameType = level.type;
    ringsControlHintLabel->setVisible(gameType == GameType::RINGS_CONGROL && !forScreenshot);
    
    if (!forScreenshot) {
        if (GameManager::getInstance()->getCompletedLevelsCount() < level.number) {
            Analytics::getInstance()->sendEvent(FirebaseAnalytics::EventLevelUp, {{"level", ::StringUtils::toString(level.number)}});
        } else {
            Analytics::getInstance()->sendEvent("replay_level", {{"level", ::StringUtils::toString(level.number)}});
        }
        gamesPlayed++;
        GameManager::getInstance()->incrementAttemptsForCurrentLevel();
    }
    
    // Cleanup
    bullets.clear();
    gameplayNode->removeAllChildren();
    
    // UI
    levelNumberLabel->setString(::StringUtils::toString(level.number));
    if (forScreenshot) {
        levelNumberLabel->setSystemFontSize(levelNumberLabel->getSystemFontSize() / screenshotScale);
        levelNumberLabel->setScale(screenshotScale);
    }
    
    // Create objects
    GameObjectCreationHelper creator(level.gravity);
    creator.setForScreenshot(forScreenshot);
    
    // Orbits
    int zOrder = 100;

    const float centreRadius = floorf(gameScale * (level.centreRadius - 0.01f));
    
    const float orbitsSpacing = 5.0f;
    
    cocos2d::Vector<OrbitNodeHelper *> orbitHelpers;
    float orbitInnerRadius = centreRadius + 3.0f;
    for (OrbitDefinition orbit: level.orbits) {
        orbit.width = ceilf((orbit.width - 0.01f) * gameScale);
        
        OrbitNodeHelper *orbitHelper = OrbitNodeHelper::create();
        orbitHelper->setSpeed(orbit.speed);
        orbitHelper->setOpacity(orbit.opacity);
        
        auto segments = creator.createSegments(orbit, orbitInnerRadius);
        for (auto segment: segments) {
            segment->setPosition(centreMarker->getPosition());
            segment->setLocalZOrder(zOrder);
            gameplayNode->addChild(segment);
            orbitHelper->addSegment(segment);
            
            this->segments.pushBack(segment);
        }

        orbitInnerRadius += orbit.width + orbitsSpacing;
        orbitHelpers.pushBack(orbitHelper);
        zOrder--;
    }
    
    // Bullets
    int bulletNumber = 0;
    for (const Bullet &bullet: level.bullets) {
        BulletNode *bulletNode = creator.createBullet(bullet, gameScale);
        if (forScreenshot) {
            bulletNode->setPosition(getPositionForBullet(bulletNumber));
        } else {
            bulletNode->setPosition(getContentSize().width / 2, -20);
        }
        if (forScreenshot && bulletNumber == 0) {

        } else {
            if (forScreenshot) {
                bulletNode->setScale(bulletNode->getScale() * 0.65f);
            } else {
                bulletNode->setScale(0.65f * gameScale);
            }
        }
        gameplayNode->addChild(bulletNode);
        bullets.pushBack(bulletNode);
        bulletNumber++;
    }
    
    // first bullet at the end of the list
    bullets.reverse();
    
    //gray out bullets & colors
    if (!forScreenshot && AppSettings::getInstance()->isColorblindMode()) {
        std::vector<Color3B> colorBlindColors = {
            Color3B(12, 71, 81), Color3B(167, 81, 252), Color3B(56, 253, 59)};
        BulletNode *ppBullet = nullptr, *pBullet = nullptr;
        
        for (BulletNode *bullet: bullets) {
            cocos2d::Color3B colorblindColor;
            
            if (ppBullet && ppBullet->getBulletColor() == bullet->getBulletColor()) {
                colorblindColor = ppBullet->getColorblindColor();
            
            } else if (pBullet && pBullet->getBulletColor() == bullet->getBulletColor()) {
                colorblindColor = pBullet->getColorblindColor();
            
            } else {
                size_t colorIdx = 0;
                if (ppBullet) {
                    for (colorIdx = 0; colorIdx < colorBlindColors.size(); ++colorIdx) {
                        if (ppBullet->getColorblindColor() != colorBlindColors[colorIdx]
                            && pBullet->getColorblindColor() != colorBlindColors[colorIdx]) {
                            break;
                        }
                    }
                } else if (pBullet) {
                    colorIdx = 1;
                } else {
                    colorIdx = 0;
                }
                colorblindColor = colorBlindColors[colorIdx];
            }
 
            bullet->setColor(colorblindColor);
            bullet->setColorblindColor(colorblindColor);
            
            ppBullet = pBullet;
            pBullet = bullet;
        }
        
        highlightNextColors();
    }
    
    // Center
    Node *centreCircle = creator.createCentralNodeWithRadius(centreRadius, centreMarker->getPosition());
    centreCircle->setLocalZOrder(101);
    gameplayNode->addChild(centreCircle);
    
    useTimeout = level.timeout > 1;
    timeout = level.timeout;
    timeoutLabel->setVisible(useTimeout);

    float minStartDelay = 0.5f;
    if (animateAppear) {
        int orbitNumber = 1;
        for (auto &&orbit: orbitHelpers) {
            for (auto segment: orbit->getSegments()) {
                Node *innerNdode = segment->getInnerNode();
                const Vec2 originalPos = innerNdode->getPosition();
                innerNdode->setOpacity(0);
                innerNdode->setScale(powf(1.75f, orbitNumber));
                
                auto delay = DelayTime::create(orbitNumber * 0.15f);
                auto fadeIn = EaseInOut::create(FadeIn::create(0.35f), 1.25f);
                auto downscale = EaseInOut::create(ScaleTo::create(0.35f, 1.0f), 1.25f);
                auto seq = Sequence::create(delay, Spawn::create(fadeIn, downscale, NULL), NULL);
                innerNdode->runAction(seq);
            }
            
            orbitNumber += 1;
        }
        minStartDelay = 0.15f * orbitHelpers.size() + 0.35f + 0.05f;
    }
    
    if (!forScreenshot) {
        PopupsCounter::getInstance()->incrementPlaysCount();
        
        // Physics
        CC_SAFE_RELEASE(physicsHelper);
        physicsHelper = creator.getPhysicsHelper();
        physicsHelper->retain();
        physicsHelper->setContactListener(this);
        physicsHelper->setBulletsSpeed(level.bulletsSpeed);
        physicsHelper->setOrbitHelpers(orbitHelpers);
        physicsHelper->setGameScale(gameScale);
    
        //    ChipmunkDebugNode *debugNode = ChipmunkDebugNode::create(physicsHelper->getSpace());
        //    gameplayNode->addChild(debugNode);
        gameState = GameState::PREPARE;
        if (gameType != GameType::RINGS_CONGROL) {
            physicsHelper->pause();
        }
        
        auto startGame = CallFunc::create([this, level]() {
            gameState = GameState::GAME;
            physicsHelper->resume();
            if (gameType == GameType::RINGS_CONGROL) {
                bulletsSchedule.interval = level.bulletsAutoLaunchInterval;
                bulletsSchedule.passedTime = 0.0f;
            }
        });
        
        const float prepareDuration = gameType == GameType::RINGS_CONGROL ? 0.05f : minStartDelay;
        placeBulletsAtProperPositions(prepareDuration);
        runAction(Sequence::create(DelayTime::create(0.35f), startGame, nullptr));
        
        SecondChanceScheduler::getInstance()->didStartLevel(level);
        
        if (!level.hint.empty() && lastPlayedLevel != level.number) {
            const std::string hintPath = "Hints/" + level.hint;
            auto progressHUD = ProgressHUD::create();
            progressHUD->setRootNode(this);
            progressHUD->showInfoWithStatus(LocalizedString(hintPath));
        }
        
        lastPlayedLevel = level.number;
    }
}

Vec2 GameScene::getPositionForBullet(int number) const {
    if (forScreenshot) {
        const float topPosition = centreMarker->getPositionY() - getContentSize().height * 0.43f;
        return Vec2(getContentSize().width / 2, topPosition - number * 40 * gameScale);
    }
    
    switch (gameType) {
        case GameType::BULLETS_CONTROL: {
            const float topPosition = std::min(130.0f, centreMarker->getPositionY() - 200);
            return Vec2(getContentSize().width / 2, topPosition - number * 40 * gameScale);
        }
        case GameType::RINGS_CONGROL: {
            const float yPos = centreMarker->getPositionY() - 404 - number * 40;
            const float xPos = getContentSize().width / 2;
            return Vec2(xPos, yPos);
        }
    }
    return Vec2::ZERO;
}

void GameScene::placeBulletsAtProperPositions(float duration) {
    int bulletNumber = static_cast<int>(bullets.size()) - 1;
    for (Node *bullet: bullets) {
        bullet->stopAllActions();
        if (bulletNumber == 0) {
            bullet->runAction(ScaleTo::create(duration, 1.0f * gameScale));
        }
        auto moveAction = MoveTo::create(duration, getPositionForBullet(bulletNumber));
        auto easedAction = EaseInOut::create(moveAction, 2.0f);
        bullet->runAction(easedAction);
        bulletNumber -= 1;
    }
}

void GameScene::preGameOver(Segment *segment) {
    physicsHelper->pause();
    if (segment) {
        segment->setOpacity(255);
    }
    
    gameState = GameState::GAME_OVER;
    
    const auto remainingBullets = bullets.size() + activeBullets.size();
    const bool adsEnabled = AdsManager::getInstance()->isAdsEnabled();
    if (!useTimeout && gameType == GameType::BULLETS_CONTROL && adsEnabled && SecondChanceScheduler::getInstance()->didFailAt(remainingBullets)) {
        askForSecondChance(segment);
        return;
    }

    gameOver(segment);
}

void GameScene::gameOver(Segment *segment) {
    startHintLabel->runAction(FadeOut::create(0.3f));;
    
    // remove segments
    Vector<Segment *> allSegments;
    auto orbitHelpers = physicsHelper->getOrbitHelpers();
    for (auto &&orbitHelper: orbitHelpers) {
        allSegments.pushBack(orbitHelper->getSegments());
    }
    for (auto it = allSegments.begin(); it != allSegments.end(); ) {
        if ((*it)->isActive() && (*it) != segment) {
            ++it;
        } else {
            it = allSegments.erase(it);
        }
    }
    
    allSegments.reverse();
    float delay = 0.15f;
    const float interDelay = std::min(1.0f / allSegments.size(), 0.09f);
    for (Segment *segment: allSegments) {
        segment->setOpacity(255);
        auto fadeOut = FadeOut::create(interDelay * 2);
        auto delayAction = DelayTime::create(delay);
        auto removeFromParent = RemoveSelf::create();
        segment->runAction(Sequence::create(delayAction, fadeOut, removeFromParent, nullptr));
        delay += interDelay;
    }
    
    // remove bullets
    auto bullets = this->bullets;
    bullets.reverse();
    float bulletDelay = 0.0f;
    for (Node *bullet: bullets) {
        auto fadeOut = FadeOut::create(0.3f);
        auto delayAction = DelayTime::create(bulletDelay);
        bullet->runAction(Sequence::create(delayAction, fadeOut, nullptr));
        bulletDelay += 0.1f;
    }

    // final animation
    scheduleOnce([this](float) {
        CCBAnimationRunner::run(this, "GameOver", [this]() {
            auto menuScene = createDefeatMenuScene();
            if (AdsManager::getInstance()->fireEvent()){
                PopupsCounter::getInstance()->resetPlaysWithoutPopups();
            }
            
            auto transition = TransitionCrossFade::create(0.15, menuScene);
            Director::getInstance()->replaceScene(transition);
        });
    }, delay + 0.45f, "Replacement");
    
    PLAudioEngine::getInstance()->playEffect(R::Audio::LOSE);
    AchievementsTracker::getInstance()->onAttemptCompleted();
    AchievementsTracker::getInstance()->reportAll();
}

Scene *GameScene::createDefeatMenuScene() const {
    auto scene = MainMenuScene::createScene(MenuStyle::DEFEAT);
    MainMenuScene *menu = dynamic_cast<MainMenuScene *>(scene->getChildByTag(MainMenuScene::MAIN_MENU_TAG));
    menu->setScore(currentLevelNumber);
    return scene;
}

void GameScene::victory() {
    startHintLabel->runAction(FadeOut::create(0.3f));
    
    const int levelNumber = GameManager::getInstance()->getCurrentLevelNumber();
    RateScheduler::getInstance()->setEventsCount(levelNumber);
       
    GameKit::GameCenter::getInstance()->submitScore(levelNumber, AppSettings::getInstance()->getLeaderboardName());
    GameManager::getInstance()->setLevelCompleted(levelNumber);
    GameManager::getInstance()->startNextLevel();
    
    gameState = GameState::GAME_OVER;
    CCBAnimationRunner::run(this, "Victory", [this]() {
        auto menuScene = MainMenuScene::createScene(MenuStyle::VICTORY);
        MainMenuScene *menu = dynamic_cast<MainMenuScene *>(menuScene->getChildByTag(MainMenuScene::MAIN_MENU_TAG));
        menu->setScore(currentLevelNumber);
        const bool adsOnVictory = AppSettings::getInstance()->getBool(R::Config::Ads::ON_VICTORY);
        if (this->showRateDialog(menuScene)) {

        } else if (adsOnVictory && AdsManager::getInstance()->fireEvent()) {
            PopupsCounter::getInstance()->resetPlaysWithoutPopups();
        }

        auto transition = TransitionCrossFade::create(0.15, menuScene);
        Director::getInstance()->replaceScene(transition);
    });
    
    PLAudioEngine::getInstance()->playEffect(R::Audio::WIN);
    AchievementsTracker::getInstance()->onLevelCompleted(levelNumber);
    AchievementsTracker::getInstance()->onAttemptCompleted();
    AchievementsTracker::getInstance()->reportAll();
}


#pragma mark -
#pragma mark Game

void GameScene::touchOrbits() {
//    startHintLabel->setVisible(false);
    auto orbitHelpers = physicsHelper->getOrbitHelpers();
    for (auto &&orbitHelper: orbitHelpers) {
        orbitHelper->onTouch();
    }
}

void GameScene::sendBullet() {
    if (physicsHelper && !bullets.empty()) {
        auto bullet = bullets.back();
        physicsHelper->sendBullet(bullet);
        activeBullets.pushBack(bullet);
        bullets.popBack();
        placeBulletsAtProperPositions(0.2);
        
        PLAudioEngine::getInstance()->playEffect(R::Audio::TAP);        
    }
}

void GameScene::onBulletCollideWithSegment(BulletNode *bullet, Segment *segment) {
    const auto bulletColor = bullet->getBulletColor();
    const auto segmentColor = segment->getSegmentColor();
    if (bulletColor == segmentColor) {
        AchievementsTracker::getInstance()->onSegmentDestroyed();
        
        activeBullets.eraseObject(bullet);
        
        physicsHelper->removeNode(segment, true);
        
        auto fadeTo = FadeTo::create(0.07f, 0.0f);
        segment->runAction(Sequence::create(fadeTo, RemoveSelf::create(), nullptr));
        
        if (AppSettings::getInstance()->isColorblindMode()) {
            highlightNextColors();
        }
        
        if (bullets.empty() && activeBullets.empty()) {
            victory();
        }
        
    } else {
        preGameOver(segment);
    }
}

void GameScene::onBulletCollideWithCentre(BulletNode *bullet, Node *centre) {
    preGameOver(nullptr);
}

void GameScene::updateTimeoutLabel() {
    const std::string text = fmt::format(timeoutFormat, timeout);
    timeoutLabel->setString(text);
}


#pragma mark - Colorblind

void GameScene::highlightNextColors() {
    static const int GRAY_OUT_ACTION_TAG = 243;
    static const int COLORIFY_ACTION_TAG = 244;
    
    Vector<Segment *> toGrayout;
    std::vector<std::pair<Segment *, Color3B>> toColorify;
    
    for (auto segment: segments) {
        if (segment->getColor() != GRAY_COLOR && segment->getActionByTag(GRAY_OUT_ACTION_TAG) == nullptr) {
            toGrayout.pushBack(segment);
        }
    }
    
    const ssize_t colorifyCount = std::min(bullets.size(), (ssize_t)3);
    for (ssize_t i = 0; i < colorifyCount; ++i) {
        BulletNode *bullet = bullets.at(bullets.size() - 1 - i);
        const Color3B nextColor = bullet->getBulletColor();
        
        for (auto segment: segments) {
            if (segment->getSegmentColor() == nextColor) {
                auto it = toGrayout.find(segment);
                if (it != toGrayout.end()) {
                    toGrayout.erase(it);
                }
                
                if (segment->getActionByTag(COLORIFY_ACTION_TAG) == nullptr) {
                    toColorify.push_back(std::make_pair(segment, bullet->getColorblindColor()));
                }
            }
        }
    }
    
    for (Segment *segment: toGrayout) {
        segment->stopAllActionsByTag(COLORIFY_ACTION_TAG);
        ActionInterval *tint = TintTo::create(0.2f, GRAY_COLOR);
        tint->setTag(GRAY_OUT_ACTION_TAG);
        segment->runAction(tint);
        
        ActionInterval *shadowTint = SegmentShadowTintTo::create(0.2f, SHADOW_GRAY_COLOR);
        shadowTint->setTag(GRAY_OUT_ACTION_TAG);
        segment->runAction(shadowTint);
    }
    
    for (auto entry: toColorify) {
        entry.first->stopAllActionsByTag(GRAY_OUT_ACTION_TAG);
        const Color3B color = entry.second;
        ActionInterval *tint = TintTo::create(0.2f, color);
        tint->setTag(COLORIFY_ACTION_TAG);
        entry.first->runAction(tint);
        
        const Color3B shadowColor(entry.second.r * 0.87f, entry.second.g * 0.87f, entry.second.b * 0.87f);
        ActionInterval *shadowTint = SegmentShadowTintTo::create(0.2f, shadowColor);
        shadowTint->setTag(COLORIFY_ACTION_TAG);
        entry.first->runAction(shadowTint);
    }
}


#pragma mark - Cross Promo

bool GameScene::showRateDialog(cocos2d::Scene *scene) {
    if (gamesPlayed >= 3 && RateScheduler::getInstance()->shouldPromptForRate() && random(0.0f, 1.0f) > 0.5f) {
        bool shownSystemDialog = false;
        
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS 
        if (AppSettings::getInstance()->getBool(R::Config::NEW_SYSTEM_RATE_ENABLED)) {
            const int newPromptsCount = UserDefault::getInstance()->getIntegerForKey("NewRateDialogPromptsCount", 0);
            const std::string lastPromptedAppVersion = UserDefault::getInstance()->getStringForKey("LastPromptedAppVersion", "");
            const std::string currentAppVersion = AppUtils::getVersionName();

            //at first prompt use custom dialog
            bool notFirstPrompt = newPromptsCount > 0;
            //don't show if more than 3 attempts & app was updated
            bool attemptsOver = newPromptsCount > 3 && lastPromptedAppVersion != currentAppVersion;
            if (notFirstPrompt && !attemptsOver) {
                shownSystemDialog = AppUtils::requestSystemReview();
                UserDefault::getInstance()->setStringForKey("LastPromptedAppVersion", currentAppVersion);
            }
            UserDefault::getInstance()->setIntegerForKey("NewRateDialogPromptsCount", newPromptsCount + 1);
        }
#endif
        if (!shownSystemDialog) {
            auto rateDialog = RateDialog::createNode();
            scene->addChild(rateDialog);
        }
        
        PopupsCounter::getInstance()->resetPlaysWithoutPopups();
        RateScheduler::getInstance()->setJustPromptedForRate();

        return true;
    }
    return false;
}


#pragma mark - Second Chance

void GameScene::askForSecondChance(Segment *segment) {
    PopupsCounter::getInstance()->resetPlaysWithoutPopups();
    
    Analytics::getInstance()->sendEvent("show_second_chance");
    SecondChanceScheduler::getInstance()->didShow();
    auto dialog = SecondChanceDialog::createFromCcb();
    dialog->setCompletion([this, segment](bool success) {
        if (success) {
            Analytics::getInstance()->sendEvent("use_second_chance");
            resurrect();
            SecondChanceScheduler::getInstance()->didAccept();
            AchievementsTracker::getInstance()->onSecondChanceUsed();
        } else {
            gameOver(segment);
            SecondChanceScheduler::getInstance()->didDecline();
        }
    });
    addChild(dialog);
}

void GameScene::resurrect() {
    for (auto bullet: activeBullets) {
        bullets.pushBack(bullet);
        physicsHelper->makeBulletKinematic(bullet);
    }
    activeBullets.clear();
    placeBulletsAtProperPositions(0.5f);
    gameState = GameState::GAME;
    physicsHelper->resume();
}

#pragma mark -
#pragma mark Pause

void GameScene::showPauseDialog() {
    if (gameState != GameState::GAME || !physicsHelper) {
        return;
    }
    gameState = GameState::PAUSE;
    physicsHelper->pause();
    
    PauseDialog *pauseDialog = PauseDialog::createFromCcb();
    pauseDialog->setCompletion([this](bool resume) {
        if (resume) {
            gameState = GameState::GAME;
            physicsHelper->resume();
            
        } else {
            auto menuScene = MainMenuScene::createScene(MenuStyle::MAIN_MENU);
            const Color3B transitionColor =AppSettings::getInstance()->isNightModeEnabled() ? Color3B::BLACK : Color3B::WHITE;
            auto transition = TransitionFade::create(0.3, menuScene, transitionColor);
            Director::getInstance()->replaceScene(transition);
        }
    });
    addChild(pauseDialog);
}
