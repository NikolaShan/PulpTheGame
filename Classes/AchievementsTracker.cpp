//
//  AchievementsTracker.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 18/02/16.
//
//

#include "base/CCUserDefault.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"
#include "Utils/TimeUtils.h"

#include "AchievementsTracker.h"
#include "GameCenter.h"
#include "AppSettings.h"
#include "Game/GameManager.h"
#include "Resources/ConfigKeys.h"
#include "FirebaseAnalytics.h"

USING_NS_CC;

void AchievementsTracker::onLevelCompleted(int level) {
    pendingCompletedLevel = level;
}

void AchievementsTracker::onAttemptCompleted() {
    incrementAttemptsCount();
}

void AchievementsTracker::onSegmentDestroyed() {
    incrementDestroyedSegmentsCount();
}

void AchievementsTracker::reportAll() {
    std::vector<int> steps;
    std::vector<int> required;
    std::vector<std::string> names;
    
    //completed level
    static std::vector<int> targetLevels = {5, 20, 50, 100, 200};
    if (pendingCompletedLevel > 0) {
        for (int targetLevel: targetLevels) {
            steps.push_back(pendingCompletedLevel);
            required.push_back(targetLevel);
        }
        const auto levelsAchievementsNames = getAchievementNamesForKey("levels");
        names.insert(names.end(), levelsAchievementsNames.begin(), levelsAchievementsNames.end());
        
        if (pendingCompletedLevel == GameManager::getInstance()->getLevelsCount()) {
            const std::string achieveName = AppSettings::getInstance()->getString(R::Config::GameCenter::Achievements::Hidden::CHAMPION);
            GameKit::GameCenter::getInstance()->unlockAchievement(achieveName);
        }
    }
    
    //attempts
    static std::vector<int> targetAttempts = {};//{75, 150, 300, 500};
    const int totalAttempts = getAttemptsCount();
    for (int targetAttempt: targetAttempts) {
        steps.push_back(totalAttempts);
        required.push_back(targetAttempt);
    }
    const auto attemptsAchievementsNames = getAchievementNamesForKey("attempts");
    names.insert(names.end(), attemptsAchievementsNames.begin(), attemptsAchievementsNames.end());
    
    //bullets
    static std::vector<int> targetDestroyed = {500};
    const int totalDestroyed = getDestroyedSegmentsCount();
    
    const auto destroyAchievementsNames = getAchievementNamesForKey("destroy");
    names.insert(names.end(), destroyAchievementsNames.begin(), destroyAchievementsNames.end());
    if (!destroyAchievementsNames.empty()) {
        for (int targetDestroy: targetDestroyed) {
            steps.push_back(totalDestroyed);
            required.push_back(targetDestroy);
        }
    }
    
    //segments
    if (totalDestroyed >= 500) {
        
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        const std::string achieveName = AppSettings::getInstance()->getString(R::Config::GameCenter::Achievements::Hidden::DESTROYER);
        GameKit::GameCenter::getInstance()->unlockAchievement(achieveName);
#endif
    }
    
    pendingCompletedLevel = -1;
    GameKit::GameCenter::getInstance()->reportAchievements(names, steps, required);
    for (size_t i = 0; i < names.size(); ++i) {
        if (steps[i] >= required[i]) {
            UserDefault *ud = UserDefault::getInstance();
            const std::string key = "analytics_" + names[i];
            if (!ud->getBoolForKey(key.c_str())) {
                Analytics::getInstance()->sendEvent(
                    FirebaseAnalytics::EventUnlockAchievement, {{"achievement_id", names[i]}});
                ud->setBoolForKey(key.c_str(), true);
            }
        }
    }
}

void AchievementsTracker::onGameCompleted() {
    const std::string achieveName = AppSettings::getInstance()->getString(R::Config::GameCenter::Achievements::Hidden::CHAMPION);
    GameKit::GameCenter::getInstance()->unlockAchievement(achieveName);
}

void AchievementsTracker::onSocialAction() {
    const std::string key = "GKSCLCTNS";
    const int actionsCount = UserDefault::getInstance()->getIntegerForKey(key.c_str(), 0) + 1;
    UserDefault::getInstance()->setIntegerForKey(key.c_str(), actionsCount);
    if (actionsCount >= 1) {//always true for now
        const std::string achieveName = AppSettings::getInstance()->getString(R::Config::GameCenter::Achievements::Hidden::SOCIAL);
        GameKit::GameCenter::getInstance()->unlockAchievement(achieveName);
    }
}

std::vector<std::string> AchievementsTracker::getAchievementNamesForKey(const std::string &key) const {
    return AppSettings::getInstance()->getStringVector(R::Config::GameCenter::Achievements::PATH + "_" + key);
}

void AchievementsTracker::onSecondChanceUsed() {
    const std::string key = "GKSCNDCHNCACCPTD";
    const int usedCount = UserDefault::getInstance()->getIntegerForKey(key.c_str(), 0) + 1;
    UserDefault::getInstance()->setIntegerForKey(key.c_str(), usedCount);
    if (usedCount >= 5) {
        const std::string achieveName = AppSettings::getInstance()->getString(R::Config::GameCenter::Achievements::Hidden::LAST_CHANCE);
        GameKit::GameCenter::getInstance()->unlockAchievement(achieveName);
    }
}

int AchievementsTracker::getAttemptsCount() {
    return UserDefault::getInstance()->getIntegerForKey("TTLATTMPTS", 0);
}

void AchievementsTracker::incrementAttemptsCount() {
    UserDefault::getInstance()->setIntegerForKey("TTLATTMPTS", getAttemptsCount() + 1);
}

int AchievementsTracker::getDestroyedSegmentsCount() {
    return UserDefault::getInstance()->getIntegerForKey("TTLBLLTS", 0);
}

void AchievementsTracker::incrementDestroyedSegmentsCount() {
    UserDefault::getInstance()->setIntegerForKey("TTLBLLTS", getDestroyedSegmentsCount() + 1);
}

void AchievementsTracker::startTimeMeasure() {
    if (schedulerTarget) {
        return;
    }
    schedulerTarget = cocos2d::Node::create();
    schedulerTarget->retain();
    Director::getInstance()->getScheduler()->schedule([this](float dt) {
        this->incrementTotalTime(std::min(15.0f, dt));
    }, schedulerTarget, 10.0f, false, "TimeMeasure");
}

void AchievementsTracker::start() {
    startTimeMeasure();
}

void AchievementsTracker::incrementTotalTime(float dt) {
    const std::string key = "GKTTLTMPLD";
    const float totalTime = UserDefault::getInstance()->getFloatForKey(key.c_str(), 0.0f) + dt;
    UserDefault::getInstance()->setFloatForKey(key.c_str(), totalTime);
    if (totalTime >= 6.0f * TimeUtils::SECONDS_IN_HOUR) {
        const std::string achieveName = AppSettings::getInstance()->getString(R::Config::GameCenter::Achievements::Hidden::LONG_SESSION);
        GameKit::GameCenter::getInstance()->unlockAchievement(achieveName);
    }
}
