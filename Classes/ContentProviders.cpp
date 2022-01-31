//
//  ContentProvider.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 23/07/15.
//
//

#include "ContentProviders.h"
#include "Advertise.h"
#include "Analytics.h"
#include "Game/GameManager.h"
#include "MainMenuScene.h"
#include "base/CCDirector.h"
#include "UI/UIProgressHUD.h"
#include "Utils/Localization.h"
#include "SchedulerHelper.h"
#include "Resources/Strings.h"

#pragma mark - IAPs

const std::string IAPContentProvider::NO_ADS_REWARD = "disable_ads";
const std::string IAPContentProvider::SKIPS_REWARD = "skips";

void IAPContentProvider::provideContent(const std::string &reward, const cocos2d::Value &value) {
    if (reward == NO_ADS_REWARD) {
        AdsManager::getInstance()->setAdsEnabled(false);
    } else if (reward == SKIPS_REWARD) {
        const int currentSkipsCount = GameManager::getInstance()->getAvailableLevelSkips();
        GameManager::getInstance()->setAvailableLevelSkips(currentSkipsCount + value.asInt());
    }
}

bool IAPContentProvider::hasReward(const std::string &reward) const {
    return reward == NO_ADS_REWARD || reward == SKIPS_REWARD;
}

#pragma mark - Notifications

void NotificationHandler::hanleNotification(int tag, const cocos2d::ValueMap &info){
    // Skip Level (mb need rework)
    CCLOG("Did receive notification");
    auto gameManager = GameManager::getInstance();
    const int currentLevelNumber = gameManager->getCompletedLevelsCount() + 1;
    if (currentLevelNumber <= gameManager->getLevelsCount()){
        gameManager->setLevelCompleted(currentLevelNumber);
        gameManager->setCurrentLevel(currentLevelNumber + 1);
    }
    Analytics::getInstance()->sendEvent("open_notification");
    
    std::function<void()> replaceScene = [currentLevelNumber]() {
        bool isMenu = false;
        auto runningScene = cocos2d::Director::getInstance()->getRunningScene();
        for (auto child: runningScene->getChildren()) {
            if (dynamic_cast<MainMenuScene *>(child)) {
                isMenu = true;
                break;
            }
        }
        
        if (isMenu) {
            auto menuScene = MainMenuScene::createScene(MenuStyle::MAIN_MENU);
            cocos2d::Director::getInstance()->replaceScene(menuScene);
            
            const std::string message = FormattedLocalizedString(R::Strings::LocalNotification::OPEN_MESSAGE, currentLevelNumber);
            auto progressHUD = ProgressHUD::create();
            progressHUD->setRootNode(menuScene);
            progressHUD->showSuccessWithStatus(message);
        }
    };
    
    auto runningScene = cocos2d::Director::getInstance()->getRunningScene();
    if (runningScene) {
        replaceScene();
    } else {
        SchedulerHelper::getInstance().schedule(replaceScene, 0.1f);
    }
}
