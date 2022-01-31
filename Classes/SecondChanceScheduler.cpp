//
//  SecondChanceScheduler.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 21.11.15.
//
//

#include <functional>

#include "base/CCUserDefault.h"

#include "SecondChanceScheduler.h"
#include "AppSettings.h"

USING_NS_CC;

SecondChanceScheduler::SecondChanceScheduler() {
    resetRequiredDefeatsCount();
    wasUsedAtCurrentAttempt = false;
}

void SecondChanceScheduler::didStartLevel(const LevelDefinition &level) {
    if (lastPlayedLevel != level.number) {
        lastPlayedLevel = level.number;
        resetRequiredDefeatsCount();
        requiredDefeatsBeforeShowing = AppSettings::getInstance()->getInt(R::Config::SecondChance::MIN_DEFEATS_COUNT);
        failuresDistribution.clear();
    }
    
    wasUsedAtCurrentAttempt = false;
    initialLevelBullets = static_cast<int>(level.bullets.size());
}

void SecondChanceScheduler::resetRequiredDefeatsCount() {
    requiredDefeatsBeforeShowing = AppSettings::getInstance()->getInt(R::Config::SecondChance::MIN_DEFEATS_COUNT);
}

void SecondChanceScheduler::didShow() {
    resetRequiredDefeatsCount();
}

void SecondChanceScheduler::didDecline() {
    requiredDefeatsBeforeShowing += declinesCount;
    declinesCount += 1;
}

void SecondChanceScheduler::didAccept() {
    declinesCount = 0;
    wasUsedAtCurrentAttempt = true;
}

bool SecondChanceScheduler::didFailAt(size_t remainingCount) {
    //was just used
    if (wasUsedAtCurrentAttempt) {
        return false;
    }
    
    //completed some really hard moment
    failuresDistribution.push_back(remainingCount);
    if (failuresDistribution.size() > 7) {
        const ssize_t countBelow = std::count_if(failuresDistribution.begin(), failuresDistribution.end(), [remainingCount](size_t &rc) {
            return rc <= remainingCount;
        });
        if (countBelow <= static_cast<ssize_t>(ceil(0.09 * failuresDistribution.size()))) {
            return true;
        }
    }
    
    requiredDefeatsBeforeShowing -= 1;
    
    //failed too early
    const float requiredPercent = 1.0f - AppSettings::getInstance()->getFloat(R::Config::SecondChance::PERCENT_COMPLETION);
    if (remainingCount > ceilf(requiredPercent * initialLevelBullets)) {
        return false;
    }
    
    //not got required defeats count
    if (requiredDefeatsBeforeShowing > 0) {
        return false;
    }
    
    return true;
}
