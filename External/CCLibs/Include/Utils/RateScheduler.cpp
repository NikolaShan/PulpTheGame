//
//  RateScheduler.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 16.10.15.
//
//

#include "RateScheduler.h"

#include "base/CCUserDefault.h"
#include "platform/CCPlatformConfig.h"
#include "AppUtils.h"
#include "StringUtils.h"
#include "TimeUtils.h"

USING_NS_CC;

const std::string RateScheduler::USES_COUNT_KEY = "RateSchedulerUsesCount";
const std::string RateScheduler::EVENTS_COUNT_KEY = "RateSchedulerEventsCount";
const std::string RateScheduler::DECLINED_VERSION_KEY = "RateSchedulerDeclinedVersion";
const std::string RateScheduler::RATED_VERSION_KEY = "RateSchedulerRatedVersion";
const std::string RateScheduler::INSTALL_TIME_KEY = "RateSchedulerInstallTime";
const std::string RateScheduler::LAST_PROMPT_TIME_KEY = "RateSchedulerLastPromptTime";

RateScheduler *RateScheduler::getInstance() {
    static auto instance = new RateScheduler;
    return instance;
}

RateScheduler::RateScheduler() {
    launchTime = time(nullptr);
    
    eventsUntilPrompt = 3;
    usesUntilPrompt = 2;
    daysUntilPrompt = 0.5;
    remindPeriod = 1.0;
    minIntervalFromLaunch = 30;
    
    if (getInstallTime() < 1) {
        const time_t now = time(nullptr);
        UserDefault::getInstance()->setStringForKey(INSTALL_TIME_KEY.c_str(), ::StringUtils::toString(now));
    }
    incrementUsesCount();
    CCLOG("Rate scheduler: declined version (%s), rated version (%s)", getDeclinedVersionName().c_str(), getRatedVersionName().c_str());
}

void RateScheduler::incrementUsesCount() {
    const int currentUsesCount = getUsesCount();
    UserDefault::getInstance()->setIntegerForKey(USES_COUNT_KEY.c_str(), currentUsesCount + 1);
}

int RateScheduler::getUsesCount() const {
    return UserDefault::getInstance()->getIntegerForKey(USES_COUNT_KEY.c_str(), 0);
}

void RateScheduler::incrementEventsCount() {
    const int currentEventsCount = getEventsCount();
    UserDefault::getInstance()->setIntegerForKey(EVENTS_COUNT_KEY.c_str(), currentEventsCount + 1);
}

void RateScheduler::setEventsCount(int count) {
    const int currentEventsCount = getEventsCount();
    if (count > currentEventsCount) {
        UserDefault::getInstance()->setIntegerForKey(EVENTS_COUNT_KEY.c_str(), count);
    }
}

int RateScheduler::getEventsCount() const {
    return UserDefault::getInstance()->getIntegerForKey(EVENTS_COUNT_KEY.c_str(), 0);
}

bool RateScheduler::declinedThisVersion() const {
    const std::string currentVersionName = AppUtils::getVersionName();
    return currentVersionName == getDeclinedVersionName();
}

bool RateScheduler::declinedAnyVersion() const {
    return getDeclinedVersionName() != "";
}

bool RateScheduler::ratedThisVersion() const {
    const std::string currentVersionName = AppUtils::getVersionName();
    return currentVersionName == getRatedVersionName();
}

bool RateScheduler::ratedAnyVersion() const {
    return getRatedVersionName() != "";
}

time_t RateScheduler::getInstallTime() const {
    const std::string installTime = UserDefault::getInstance()->getStringForKey(INSTALL_TIME_KEY.c_str(), "0");
    return ::StringUtils::toNumber<time_t>(installTime);
}

time_t RateScheduler::getLastPromptTime() const {
    const std::string promptTime = UserDefault::getInstance()->getStringForKey(LAST_PROMPT_TIME_KEY.c_str(), "0");
    return ::StringUtils::toNumber<time_t>(promptTime);
}

void RateScheduler::setRatedThisVersion() {
    UserDefault::getInstance()->setStringForKey(RATED_VERSION_KEY.c_str(), AppUtils::getVersionName());
}

void RateScheduler::setDeclinedThisVersion() {
    UserDefault::getInstance()->setStringForKey(DECLINED_VERSION_KEY.c_str(), AppUtils::getVersionName());
}

bool RateScheduler::shouldPromptForRate() const {
    const time_t now = time(nullptr);
    
    //passed enough time since launch
    if (now < launchTime + minIntervalFromLaunch) {
        CCLOG("RateScheduler: Should wait slightly more time after app launch");
        return false;
    }
    
    //passed enough time since installation
    const time_t installTime = getInstallTime();
    if (now < (installTime + daysUntilPrompt * TimeUtils::SECONDS_IN_DAY)) {
        CCLOG("RateScheduler: Should wait from first install: %f", (installTime + daysUntilPrompt * TimeUtils::SECONDS_IN_DAY - now) / TimeUtils::SECONDS_IN_DAY);
        return false;
    }
    
    //enough uses count
    if (getUsesCount() < getUsesUntilPrompt()) {
        CCLOG("RateScheduler: Not enough uses: %d of %d", getUsesCount(), getUsesUntilPrompt());
        return false;
    }
    
    //enough events count
    if (getEventsCount() < getEventsUntilPrompt()) {
        CCLOG("RateScheduler: Not enough events: %d of %d", getEventsCount(), getEventsUntilPrompt());
        return false;
    }
    
    //passed enough time since last remind
    const time_t lastPromptTime = getLastPromptTime();
    if (now < (lastPromptTime + remindPeriod * TimeUtils::SECONDS_IN_DAY)) {
        CCLOG("RateScheduler: Should wait from last remind: %f", (lastPromptTime + remindPeriod * TimeUtils::SECONDS_IN_DAY - now) / TimeUtils::SECONDS_IN_DAY);
        return false;
    }
    
    //declined?
    if (declinedThisVersion()) {
        CCLOG("RateScheduler: This version was declined");
        return false;
    }
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    //rated this version?
    if (ratedThisVersion()) {
        CCLOG("RateScheduler: This version was rated");
        return false;
    }
    
    //rated any version and should not repeat
    if (ratedAnyVersion() && !getPromptForNewVersionIfUserRated()) {
        CCLOG("RateScheduler: Some version was rated already, no re-prompt");
        return false;
    }
#else
    if (ratedAnyVersion()) {
        CCLOG("RateScheduler: Some version was rated already, no re-prompt for android");
        return false;
    }
#endif
    
    return true;
}

void RateScheduler::setJustPromptedForRate() {
    const time_t now = time(nullptr);
    UserDefault::getInstance()->setStringForKey(LAST_PROMPT_TIME_KEY.c_str(), ::StringUtils::toString(now));
}


#pragma mark Support

std::string RateScheduler::getDeclinedVersionName() const {
    return UserDefault::getInstance()->getStringForKey(DECLINED_VERSION_KEY.c_str(), "");
}

std::string RateScheduler::getRatedVersionName() const {
    return UserDefault::getInstance()->getStringForKey(RATED_VERSION_KEY.c_str(), "");
}