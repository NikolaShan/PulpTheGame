//
//  AchievementsTracker.hpp
//  Pulp
//
//  Created by Mikhail Shulepov on 18/02/16.
//
//

#pragma once

#include <string>
#include <vector>
#include "base/ccTypes.h"
#include "2d/CCNode.h"

class AchievementsTracker {
public:
    static AchievementsTracker *getInstance() {
        static AchievementsTracker *instance = new AchievementsTracker;
        return instance;
    }
    
    void start();
    
    void onLevelCompleted(int level);
    void onAttemptCompleted();
    void onSegmentDestroyed();
    void reportAll();
    
    void onSecondChanceUsed();
    void onSocialAction();
    void onGameCompleted();
    
private:
    void startTimeMeasure();

    int getAttemptsCount();
    void incrementAttemptsCount();
    int getDestroyedSegmentsCount();
    void incrementDestroyedSegmentsCount();
    
    void incrementTotalTime(float dt);
    
    std::vector<std::string> getAchievementNamesForKey(const std::string &key) const;
    
private:
    AchievementsTracker() {}
    ~AchievementsTracker() {
        schedulerTarget->release();
        schedulerTarget = nullptr;
    }
    
private:
    int pendingCompletedLevel = -1;
    
    cocos2d::Node *schedulerTarget = nullptr;
};