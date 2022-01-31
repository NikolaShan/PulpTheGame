//
//  Analytics.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 15/03/16.
//
//

#include "AnalyticsDeprecated.h"

#include "base/CCUserDefault.h"
#include "Utils/StringUtils.h"

const std::string Analytics::START_TIME_KEY = "A_StartTime";
const std::string Analytics::END_TIME_KEY = "A_EndTime";

void Analytics::startSession() {
    submitSessionLength();
    
    const time_t startTime = time(nullptr);
    cocos2d::UserDefault::getInstance()->setStringForKey(START_TIME_KEY.c_str(), ::StringUtils::toString(startTime));
    cocos2d::UserDefault::getInstance()->setStringForKey(END_TIME_KEY.c_str(), "0");
}

//if passed time less than 60 seconds - continue session
//else submit all session and start new
bool Analytics::startOrResumeSession() {
    const time_t currentTime = time(nullptr);
    const time_t endTime = ::StringUtils::toNumber<time_t>(cocos2d::UserDefault::getInstance()->getStringForKey(END_TIME_KEY.c_str(), "0"));
    time_t startTime = ::StringUtils::toNumber<time_t>(cocos2d::UserDefault::getInstance()->getStringForKey(START_TIME_KEY.c_str(), "0"));
    
    bool startedNewSession;
    if (currentTime - endTime > 30) {
        submitSessionLength();
        startTime = time(nullptr);
        startedNewSession = true;
    } else {
        startTime = startTime + (currentTime - endTime);//just substract time when app was in background
        startedNewSession = false;
    }
    cocos2d::UserDefault::getInstance()->setStringForKey(END_TIME_KEY.c_str(), ::StringUtils::toString(startTime));
    return startedNewSession;
}

void Analytics::endSession() {
    const time_t endTime = time(nullptr);
    cocos2d::UserDefault::getInstance()->setStringForKey(END_TIME_KEY.c_str(), ::StringUtils::toString(endTime));
}

void Analytics::submitSessionLength() {
    const time_t endTime = ::StringUtils::toNumber<time_t>(cocos2d::UserDefault::getInstance()->getStringForKey(END_TIME_KEY.c_str(), "0"));
    if (endTime == 0) {
        return;
    }
    const time_t startTime = ::StringUtils::toNumber<time_t>(cocos2d::UserDefault::getInstance()->getStringForKey(START_TIME_KEY.c_str(), "0"));
    const time_t passedTime = endTime - startTime;
    if (passedTime > 10) {       
        const std::vector<time_t> times = {10, 30, 60, 90, 120, 150, 180, 240, 300, 420, 600};
        time_t low = 0, high = 3000;
        for (auto time: times) {
            if (time > passedTime) {
                high = time;
                break;
            }
            low = time;
        }
        const std::string passedTimeStr = ::StringUtils::toString(low) + "-" + ::StringUtils::toString(high);
        Analytics::getInstance()->sendEvent("Session", passedTimeStr, "", static_cast<int>(passedTime));
        
    } else {
        Analytics::getInstance()->sendEvent("Session", "0-10", "", static_cast<int>(passedTime));
    }
    cocos2d::UserDefault::getInstance()->setStringForKey(END_TIME_KEY.c_str(), "0");
}