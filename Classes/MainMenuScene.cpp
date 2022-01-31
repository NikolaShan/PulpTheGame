//
//  MainMenuScene.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 17/07/15.
//
//

#include "2d/CCLabel.h"
#include "2d/CCScene.h"
#include "2d/CCTransition.h"
#include "base/CCDirector.h"
#include "cocosbuilder/CocosBuilder.h"
#include "platform/CCApplication.h"
#include "Utils/PLAudioEngine.h"

#include "UI/CCBButton.h"
#include "UI/UIProgressHUD.h"
#include "Utils/GuiUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/RateScheduler.h"
#include "UI/CCBAnimationRunner.h"
#include "Resources/Resources.h"
#include "Resources/Strings.h"
#include "Game/GameManager.h"
#include "Twitter.h"
#include "Facebook.h"
#include "ShareManager.h"
#include "GameCenter.h"
#include "IAPurchasesManager.h"
#include "Analytics.h"
#include "FirebaseAnalytics.h"
#include "Advertise.h"
#include "CrashlyticsTracker.h"

#include "AchievementsTracker.h"
#include "MainMenuScene.h"
#include "GameScene.h"
#include "InfiniteGameScene.h"
#include "AppSettings.h"
#include "RemoveAdsDialog.h"
#include "LevelSelectionScene.h"
#include "SkipLevelDialog.h"
#include "PopupsCounter.h"
#include "ExitConfirmationDialog.h"
#include "ShareBanner.h"
#include "CCBNodeContainer.h"
#include "SettingsDialog.h"

USING_NS_CC;
using namespace cocosbuilder;
using namespace extension;

#define SETUP_CUSTOM_CCBBUTTON_AND_ASSIGN(_TARGET_NAME_, _TITLE_, _FUNC_NAME_, _VAR_NAME_)\
if (_TARGET_NAME_ == memberVariableName) {\
    CCBButton *btn = dynamic_cast<CCBButton *>(node);\
    CC_ASSERT(btn);\
    btn->setTitle(_TITLE_);\
    btn->setCallback([this](){_FUNC_NAME_();});\
    _VAR_NAME_ = btn;\
    _VAR_NAME_->retain();\
    return true;\
}

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#define USE_ANDROID_UI 1
#endif

static MenuStyle sMenuStyle;

Scene *MainMenuScene::createWithCurrentStyle() {
    return createScene(sMenuStyle);
}

Scene *MainMenuScene::createScene(MenuStyle style) {
    sMenuStyle = style;
    
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("MainMenuScene", MainMenuSceneLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("GameScene", GameSceneLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("NodesContainer", CCBNodeContainerLoader::loader());
    
    auto scene = Scene::create();
    {
        auto ccbReader = new CCBReader(nodeLoaderLibrary);
        Node *rootNode = ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::MainMenu::BACKGROUND));
        ccbReader->release();
        scene->addChild(rootNode);
    }
    {
        const auto screenSize = Director::getInstance()->getWinSize();
        const float desiredHeight = AppUtils::isIPhoneX() ? 600.0f : 540.0f;
        const cocos2d::Size desiredSize(screenSize.width, std::min(desiredHeight, screenSize.height));
        
        auto ccbReader = new CCBReader(nodeLoaderLibrary);
        std::string sceneFile = R::UI::MainMenu::SCENE.c_str();
        if (AppUtils::isTablet()) {
            sceneFile = R::UI::MainMenu::SCENE_TABLET;
        }

        Node *rootNode = ccbReader->readNodeGraphFromFile(UI_FILE(sceneFile), nullptr, desiredSize);
        rootNode->setTag(MAIN_MENU_TAG);
        ccbReader->release();
       // if (AppUtils::isIPhoneX()) {
        //    rootNode->setPositionY(screenSize.height - desiredSize.height - 20.0f);
        //} else {
            rootNode->setPositionY(screenSize.height - desiredSize.height);
        //}
        scene->addChild(rootNode);
    }
    return scene;
}

MainMenuScene::MainMenuScene(): lhsButton(nullptr), rhsButton(nullptr), buttonsContainer(nullptr) {
    GuiUtils::setupEscapePressHandler(this, [this]() {
        if (sMenuStyle == MenuStyle::MAIN_MENU) {
            getParent()->addChild(ExitConfirmationDialog::createFromCcb());
        } else {
            openMainMenu();
        }
    });
}

MainMenuScene::~MainMenuScene() {
    CC_SAFE_RELEASE(lhsButton);
    CC_SAFE_RELEASE(rhsButton);
    CC_SAFE_RELEASE(buttonsContainer);
}

bool MainMenuScene::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    auto gm = GameManager::getInstance();
    
    ASSIGN_VARIABLE("ButtonsContainer", buttonsContainer, CCBNodeContainer);
    
    SETUP_CUSTOM_CCBBUTTON("PlayButton",
        FormattedLocalizedString(R::Strings::Menu::LEVEL_NUMBER, GameManager::getInstance()->getCurrentLevelNumber()),
        play);
    
    SETUP_CUSTOM_CCBBUTTON("MenuButton", "", openMainMenu);
    SETUP_CUSTOM_IMAGE_CCBBUTTON("ShareTwitter", R::Images::Menu::TWITTER_IMG, shareTwitter);
    SETUP_CUSTOM_IMAGE_CCBBUTTON("ShareFacebook", R::Images::Menu::FACEBOOK_IMG, shareFacebook);
    SETUP_CUSTOM_IMAGE_CCBBUTTON("SettingsButton", R::Images::Menu::SETTINGS_IMG, showSettings);

    SETUP_CUSTOM_IMAGE_CCBBUTTON("RateButton", R::Images::Menu::RATE_IMG, rate);

#ifdef USE_ANDROID_UI
    SETUP_CUSTOM_IMAGE_CCBBUTTON("LeaderboardButton", R::Images::Menu::LEADERBOARD_ANDROID_IMG, showLeaderboards);
    SETUP_CUSTOM_IMAGE_CCBBUTTON("AchievementsButton", R::Images::Menu::ACHIEVEMENTS_ANDROID_IMG, showAchievements);
    SETUP_CUSTOM_IMAGE_CCBBUTTON("GameCenterButton", R::Images::Menu::GOOGLE_GAMEPAD_IMG, toggleGameServices);
#else
    SETUP_CUSTOM_IMAGE_CCBBUTTON("LeaderboardButton", R::Images::Menu::LEADERBOARD_IMG, showLeaderboards);
#endif
   
    SETUP_CUSTOM_IMAGE_CCBBUTTON("NoAdsButton", R::Images::Menu::NO_ADS_IMG, askDisableAds);
    
    if (memberVariableName == "ShareButton") {
        CCBButton *btn = dynamic_cast<CCBButton *>(node);
        btn->setIcon(R::Images::Menu::SHARE_IMG);
        btn->setCallback([this, btn](){share(btn);});
        return true;
    }
    
    switch (sMenuStyle) {
        case MenuStyle::MAIN_MENU: {
            SETUP_CUSTOM_CCBBUTTON_AND_ASSIGN("LeftButton", LocalizedString(R::Strings::Menu::LEVELS), chooseLevel, lhsButton);
            SETUP_CUSTOM_CCBBUTTON_AND_ASSIGN("RightButton", LocalizedString(R::Strings::Menu::ENDLESS), playEndless, rhsButton);
            break;
        }
        case MenuStyle::DEFEAT: {
            SETUP_LABEL_MAX_WIDTH("LevelFailedTitle", LocalizedString(R::Strings::Menu::LEVEL_FAILED), 290);
            SETUP_CUSTOM_CCBBUTTON_AND_ASSIGN("LeftButton", LocalizedString(R::Strings::Menu::SKIP), showSkipOptions, lhsButton);
            SETUP_CUSTOM_CCBBUTTON_AND_ASSIGN("RightButton", LocalizedString(R::Strings::Menu::RETRY), play, rhsButton);
            break;
        }
        case MenuStyle::VICTORY: {
            const bool gameCompleted = gm->getRecentlyPlayedLevelNumber() == gm->getLevelsCount();
            if (gameCompleted) {
                SETUP_LABEL_MAX_WIDTH("LevelSuccessTitle", LocalizedString(R::Strings::Menu::GAME_COMPLETED), 290);
            } else {
                SETUP_LABEL_MAX_WIDTH("LevelSuccessTitle", LocalizedString(R::Strings::Menu::LEVEL_SUCCESS), 290);
            }
            if (memberVariableName == "LeftButton") {
                CCBButton *btn = dynamic_cast<CCBButton *>(node);
                lhsButton = btn;
                btn->retain();
                if (random(0.0f, 1.0f) > 0.5f || RateScheduler::getInstance()->ratedAnyVersion() || gameCompleted) {
                    btn->setTitle(LocalizedString(R::Strings::Menu::SHARE));
                    btn->setCallback([this, btn](){share(btn);});
                } else {
                    btn->setTitle(LocalizedString(R::Strings::Menu::RATE));
                    btn->setCallback([this](){
                        RateScheduler::getInstance()->setRatedThisVersion();
                        rate();
                    });
                }
                return true;
            }
            
            if (gameCompleted) {
                SETUP_CUSTOM_CCBBUTTON_AND_ASSIGN("RightButton", LocalizedString(R::Strings::Menu::RATE), rate, rhsButton);
            } else {
                SETUP_CUSTOM_CCBBUTTON_AND_ASSIGN("RightButton", LocalizedString(R::Strings::Menu::NEXT), play, rhsButton);
            }
            break;
        }
        case MenuStyle::ENDLESS_DEFEAT: {
            SETUP_LABEL_MAX_WIDTH("LevelSuccessTitle", LocalizedString(R::Strings::Menu::YOUR_SCORE), 290)
            if (memberVariableName == "LeftButton") {
                CCBButton *btn = dynamic_cast<CCBButton *>(node);
                lhsButton = btn;
                btn->retain();
                btn->setTitle(LocalizedString(R::Strings::Menu::SHARE));
                btn->setCallback([this, btn](){share(btn);});
                return true;
            }
            SETUP_CUSTOM_CCBBUTTON_AND_ASSIGN("RightButton", LocalizedString(R::Strings::Menu::RETRY), playEndless, rhsButton);
            break;
        }
    }
    
    if (memberVariableName == "MainMenuStyle") {
        node->setVisible(sMenuStyle == MenuStyle::MAIN_MENU);
        return true;
    }
    if (memberVariableName == "DefeatStyle") {
        node->setVisible(sMenuStyle == MenuStyle::DEFEAT);
        return true;
    }
    if (memberVariableName == "SuccessStyle") {
        node->setVisible(sMenuStyle == MenuStyle::VICTORY || sMenuStyle == MenuStyle::ENDLESS_DEFEAT);
        return true;
    }
    ASSIGN_SMART_VARIABLE("PrevLevelNumber", scoreLabel, Label)
    
    return false;
}

void MainMenuScene::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
    auto lhsButtonLabel = lhsButton->getTitleLabel();
    auto rhsButtonLabel = rhsButton->getTitleLabel();
    const float maxWidth = std::max(lhsButtonLabel->getBoundingBox().size.width, rhsButtonLabel->getBoundingBox().size.width);
    const float scale = (lhsButton->getContentSize().width - 10) / maxWidth;
    if (scale < 1) {
        lhsButtonLabel->setScale(scale);
        rhsButtonLabel->setScale(scale);
    }
    if (buttonsContainer) {
        auto nodes = buttonsContainer->getInnerNodes();
        for (auto &&entry: nodes) {
            onAssignCCBMemberVariable(this, entry.first, entry.second);
        }
    }
}

void MainMenuScene::onEnter() {
    Node::onEnter();
    
    static bool shouldShowAdsPopup = false;
    const bool adsEnabled = AdsManager::getInstance()->isAdsEnabled();
    if (GameManager::getInstance()->getAvailableLevelSkips() <= 0 && adsEnabled) {
        const std::vector<std::string> attemptsVec = AppSettings::getInstance()->getStringVector(R::Config::ATTEMPTS_TO_SKIP_FOR_VIDEO);
        std::vector<int> attemptsToSuggestSkip;
        for (auto &value: attemptsVec) {
            attemptsToSuggestSkip.push_back(::StringUtils::toNumber<int>(value));
        }
        
        const int currentAttemptsCount = GameManager::getInstance()->getAttemptsCountForCurrentLevel();
        if (SkipLevelDialog::canSkipForRewardedVideo() &&
            std::find(attemptsToSuggestSkip.begin(), attemptsToSuggestSkip.end(), currentAttemptsCount) != attemptsToSuggestSkip.end()) {
            shouldShowAdsPopup = true;
        }
    }
    
    if (shouldShowAdsPopup && PopupsCounter::getInstance()->getPlaysCountWithoutPopups() >= 3) {
        scheduleOnce([this](float){
            showSkipForAds();
        }, 0.05f, "SkipForVideo");
        shouldShowAdsPopup = false;
    }
}

void MainMenuScene::setScore(int score) {
    this->score = score;
    scoreLabel->setString(::StringUtils::toString(score));
}

void MainMenuScene::play() {
    auto gameScene = GameScene::createScene();
    const Color3B transitionColor =AppSettings::getInstance()->isNightModeEnabled() ? Color3B::BLACK : Color3B::WHITE;
    auto transition = TransitionFade::create(0.3, gameScene, transitionColor);
    Director::getInstance()->replaceScene(transition);
}

void MainMenuScene::playEndless() {
    auto gameScene = InfiniteGameScene::createScene();
    const Color3B transitionColor =AppSettings::getInstance()->isNightModeEnabled() ? Color3B::BLACK : Color3B::WHITE;
    auto transition = TransitionFade::create(0.3, gameScene, transitionColor);
    Director::getInstance()->replaceScene(transition);
}

void MainMenuScene::showSkipForAds() {
    PopupsCounter::getInstance()->resetPlaysWithoutPopups();
    SkipLevelDialog::showSkipForVideo([this](bool success) {
        if (success) {
            play();
        }
    }, getParent());
}

void MainMenuScene::showSkipOptions() {
    SkipLevelDialog::showWithCompletion([this](bool success) {
        if (success) {
            play();
        }
    }, getParent());
}

void MainMenuScene::chooseLevel() {
    auto scene = LevelSelectionScene::createScene();
    const Color3B transitionColor =AppSettings::getInstance()->isNightModeEnabled() ? Color3B::BLACK : Color3B::WHITE;
    auto transition = TransitionFade::create(0.3, scene, transitionColor);
    Director::getInstance()->replaceScene(transition);
}

void MainMenuScene::askDisableAds() {
    if (!IA::PurchasesManager::getInstance()->isProductsLoaded()) {
        ProgressHUD *hud = ProgressHUD::create();
        hud->showLoadingWithStatus(LocalizedString(R::Strings::IA::LOADING));
        hud->setSwallowTouches(true);
        hud->retain();
        IA::PurchasesManager::getInstance()->requestProducts([hud](cocos2d::EventCustom *result) {
            Value *value = (Value *)result->getUserData();
            if (value->asBool()) {
                hud->dismiss();
                RemoveAdsDialog::show();
            } else {
                hud->showFailureWithStatus(LocalizedString(R::Strings::ERROR));
            }
            hud->release();
            
        }, this);
    } else {
        RemoveAdsDialog::show();
    }
}

void MainMenuScene::showLeaderboards() {
    GameKit::GameCenter::getInstance()->showLeaderBoard(AppSettings::getInstance()->getLeaderboardName());
}

void MainMenuScene::showAchievements() {
    GameKit::GameCenter::getInstance()->showAchievements();
}

void MainMenuScene::share(CCBButton *button) {
    CrashlyticsTracker::log("Start common share");
    
    const auto sourceRect = GuiUtils::getBoundingBoxInWorldSpace(button);
    
    const int level = GameManager::getInstance()->getCurrentLevelNumber();
    ShareBanner *banner = ShareBanner::createFromCcb();
    if (sMenuStyle == MenuStyle::ENDLESS_DEFEAT) {
        banner->setupForScoreShare(score);
    } else {
        banner->setupForLevelShare(level);
    }
    banner->takeScreenshot([sourceRect](const std::string &path) {
        const int currentLevel = GameManager::getInstance()->getCurrentLevelNumber();
        auto shareMessage = FormattedLocalizedString(R::Strings::Share::SIMPLE_SHARE_MESSAGE, currentLevel);
        ShareManager()
            .setText(shareMessage)
            ->setImage(path)
            ->setUrl(AppSettings::getInstance()->getShareUrl())
            ->setSourceRect(sourceRect)
            ->doShare([](bool completed, const std::string &sharedTo) {
                if (completed) {
                    Analytics::getInstance()->sendEvent(FirebaseAnalytics::EventShare,
                                                        {{"from", "menu"}, {"to", sharedTo}});
                    AchievementsTracker::getInstance()->onSocialAction();
                }
                CrashlyticsTracker::log("End common share");
            });
    });
}

void MainMenuScene::rate() {
    Analytics::getInstance()->sendEvent("rate", {{"from", "menu"}});
    Application::getInstance()->openURL(AppSettings::getInstance()->getRateUrl());
}

void MainMenuScene::openMainMenu() {
    auto menuScene = MainMenuScene::createScene(MenuStyle::MAIN_MENU);
    const Color3B transitionColor =AppSettings::getInstance()->isNightModeEnabled() ? Color3B::BLACK : Color3B::WHITE;
    auto transition = TransitionFade::create(0.1, menuScene, transitionColor);
    Director::getInstance()->replaceScene(transition);
}

void MainMenuScene::shareFacebook() {
    const std::string title = LocalizedString(R::Strings::Share::DIRECT_TITLE);
    const std::string message = (sMenuStyle == MenuStyle::ENDLESS_DEFEAT)
    ? FormattedLocalizedString(R::Strings::Share::DIRECT_MESSAGE_SCORE, this->score)
    : FormattedLocalizedString(R::Strings::Share::DIRECT_MESSAGE, GameManager::getInstance()->getCurrentLevelNumber());
    Social::Facebook::getInstance()->shareLink(AppSettings::getInstance()->getShareUrl(), title, "", message, [this](bool success) {
        if (success) {
            Analytics::getInstance()->sendEvent("share_facebook_directly");
            Analytics::getInstance()->sendEvent(FirebaseAnalytics::EventShare,
                                                {{"from", "menu"}, {"to", "facebook"}});
            AchievementsTracker::getInstance()->onSocialAction();
        }
    });
}

void MainMenuScene::shareTwitter() {
    CrashlyticsTracker::log("Start share twitter from menu");
    const int level = GameManager::getInstance()->getCurrentLevelNumber();
    ShareBanner *banner = ShareBanner::createFromCcb();
    if (sMenuStyle == MenuStyle::ENDLESS_DEFEAT) {
        banner->setupForScoreShare(score);
    } else {
        banner->setupForLevelShare(level);
    }
    banner->takeScreenshot([this](const std::string &filePath) {
        Social::TwitterManager::TweetData tweet;
        if (sMenuStyle == MenuStyle::ENDLESS_DEFEAT) {
            tweet.text = FormattedLocalizedString(R::Strings::Share::DIRECT_MESSAGE_SCORE, this->score);
        } else {
            tweet.text = FormattedLocalizedString(R::Strings::Share::DIRECT_MESSAGE, GameManager::getInstance()->getCurrentLevelNumber());
        }
        
        tweet.url = AppSettings::getInstance()->getShareUrl();
        tweet.imagePath = filePath;
        Social::TwitterManager::getInstance()->tweet(tweet, [this](bool success) {
            if (success) {
                Analytics::getInstance()->sendEvent("share_twitter_directly");
                Analytics::getInstance()->sendEvent(FirebaseAnalytics::EventShare,
                                                    {{"from", "menu"}, {"to", "twitter"}});
                AchievementsTracker::getInstance()->onSocialAction();
            }
            CrashlyticsTracker::log("End share twitter from menu");
        });
    });
}

void MainMenuScene::toggleGameServices() {
    if (isGameServicesExpanded) {
        makeGameServicesExpanded(false);
    } else if (!GameKit::GameCenter::getInstance()->isLocalPlayerAuthenticated()) {
        GameKit::GameCenter::getInstance()->authenticateLocalPlayer([this](bool authenticated) {
            if (authenticated) {
                this->makeGameServicesExpanded(true);
            } else {
                auto hud = ProgressHUD::create();
                hud->showFailureWithStatus(LocalizedString(R::Strings::ERROR));
            }
        });
    } else {
        makeGameServicesExpanded(!isGameServicesExpanded);
    }
}

void MainMenuScene::makeGameServicesExpanded(bool expanded) {
    if (isGameServicesExpanded != expanded) {
        isGameServicesExpanded = expanded;

        CCBButton *gkButton = dynamic_cast<CCBButton *>(buttonsContainer->getInnerNodeNamed("GameCenterButton"));
        if (!expanded) {
            CCBAnimationRunner::run(buttonsContainer, "Collapse", nullptr);
            gkButton->getIconView()->runAction(EaseInOut::create(RotateTo::create(0.3f, 0.0f), 2.0f));
            isGameServicesExpanded = false;
        } else {
            CCBAnimationRunner::run(buttonsContainer, "Expand", nullptr);
            gkButton->getIconView()->runAction(EaseInOut::create(RotateTo::create(0.3f, -90.0f), 2.0f));
            isGameServicesExpanded = true;
        }
    }
}

void MainMenuScene::showSettings() {
    SettingsDialog::show();
}
