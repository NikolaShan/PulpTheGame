//
//  InfiniteGameScene.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 05/06/16.
//
//

#include "cocosbuilder/CocosBuilder.h"
#include "2d/CCScene.h"

#include "InfiniteGameScene.h"
#include "Resources/Resources.h"
#include "Game/GameManager.h"
#include "GameCenter.h"
#include "Utils/StringUtils.h"
#include "UI/CCBAnimationRunner.h"
#include "MainMenuScene.h"
#include "AppSettings.h"


USING_NS_CC;
using namespace cocosbuilder;

Scene *InfiniteGameScene::createScene() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->unregisterNodeLoader("GameScene");
    nodeLoaderLibrary->registerNodeLoader("GameScene", InfiniteGameSceneLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    InfiniteGameScene *rootNode = dynamic_cast<InfiniteGameScene *>(ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Game::SCENE)));
    ccbReader->release();
    rootNode->forScreenshot = false;
    rootNode->prepareNextLevel(false);
    rootNode->setup();
    
    auto scene = Scene::create();
    scene->addChild(rootNode);
    return scene;
}

void InfiniteGameScene::onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) {
    GameScene::onNodeLoaded(node, nodeLoader);
}

void InfiniteGameScene::setup() {
    for (size_t i = 0; i < 3; ++i) {
        auto ccbReader = new CCBReader(NodeLoaderLibrary::getInstance());
        Node *heart = ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Game::HEART));
        
        heart->setPositionX(getContentSize().width - (heart->getContentSize().width + 6.0f) * (i + 0.5f));
        heart->setPositionY(getContentSize().height - heart->getContentSize().height * 0.5f - 3.0f);
        addChild(heart);
        
        hearts.pushBack(heart);
        
        ccbReader->release();
    }
}

void InfiniteGameScene::prepareNextLevel(bool animateAppear) {
    while(true) {
        LevelDefinition level = GameManager::getInstance()->getRandomLevel();
        if (level.type == GameType::BULLETS_CONTROL) {
            level.timeout = -1.0f;
            level.hint = "";
            level.number = score;
            prepareLevel(level, animateAppear);
            break;
        }
    }
    startHintLabel->setVisible(false);
    
    for (auto heart: hearts) {
        CCBAnimationRunner::run(heart, "Normal", nullptr);
    }
}

void InfiniteGameScene::preGameOver(Segment *segment) {
    gameState = GameState::GAME_OVER;
    physicsHelper->pause();
    
    if (attemptsCount < hearts.size()) {
        const size_t n = hearts.size() - 1 - attemptsCount;
        CCBAnimationRunner::run(hearts.at(n), "Disappear", nullptr);
    }
    
    
    attemptsCount += 1;
    scheduleOnce([this, segment](float dt) {
        if (attemptsCount >= hearts.size()) {
            gameOver(segment);
            if (score > GameManager::getInstance()->getEndlessModeHighscore()) {
                GameManager::getInstance()->setEndlessModeScore(score);
                GameKit::GameCenter::getInstance()->submitScore(score, AppSettings::getInstance()->getEndlessLeaderboardName());
            }
            
        } else {
            resurrect();
        }
    }, 0.5f, "PreGameOver");
    
}

void InfiniteGameScene::onBulletCollideWithSegment(BulletNode *bullet, Segment *segment) {
    GameScene::onBulletCollideWithSegment(bullet, segment);
    if (gameState != GameState::GAME_OVER) {
        score += 1;
    }
    levelNumberLabel->setString(::StringUtils::toString(score));
}

void InfiniteGameScene::victory() {
    score += 1;
    gameState = GameState::GAME_OVER;
    attemptsCount = 0;
    scheduleOnce([this](float) {
        prepareNextLevel(true);
    }, 0.5f, "StartNext");
}

Scene *InfiniteGameScene::createDefeatMenuScene() const {
    auto scene = MainMenuScene::createScene(MenuStyle::ENDLESS_DEFEAT);
    MainMenuScene *menu = dynamic_cast<MainMenuScene *>(scene->getChildByTag(MainMenuScene::MAIN_MENU_TAG));
    menu->setScore(score);
    return scene;
}
