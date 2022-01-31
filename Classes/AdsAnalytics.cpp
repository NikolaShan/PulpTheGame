//
//  AdsAnalytics.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 18/04/2017.
//
//

#include "AdsAnalytics.h"
#include "Analytics.h"
#include "base/CCUserDefault.h"
#include "Utils/StringUtils.h"

int AdsSessionCounter::countShownInterstitial = 0;
int AdsSessionCounter::countShownRewardedVideo = 0;

const std::string AdsSessionCounter::END_TIME_KEY = "SA_EndTime";
const time_t AdsSessionCounter::SESSION_GAP_TRESHOLD = 30 * 60;
const std::string AdsSessionCounter::INTERSTITIAL_COUNTER_KEY = "SA_InterstitialCounter";
const std::string AdsSessionCounter::REWARDED_VIDEO_COUNTER_KEY = "SA_RewardedVideoCounter";

AdsPresentReporter::AdsPresentReporter(Advertise::AppearListener *i): inner(i) {
    
}

void AdsPresentReporter::adWillShow() {
    inner->adWillShow();
}

void AdsPresentReporter::adWillHide(bool rewarded) {
    inner->adWillHide(rewarded);
    if (rewarded) {
        AdsSessionCounter::countShownRewardedVideo += 1;
        Analytics::getInstance()->sendEvent("show_rewarded_video", {});
    } else {
        AdsSessionCounter::countShownInterstitial += 1;
        Analytics::getInstance()->sendEvent("show_intersitial", {});
    }
}


void AdsSessionCounter::onAppBecameActive() {
    //check last session end time
    //if difference < 30 sec - do nothing (proceed with saved values of counters)
    //else if last session existed
    //submit counters for last session
    //reset counters
    //else
    //do nothing
    
    const time_t currentTime = time(nullptr);
    const time_t endTime = ::StringUtils::toNumber<time_t>(cocos2d::UserDefault::getInstance()->getStringForKey(END_TIME_KEY.c_str(), "0"));
    if (endTime < 1 || (currentTime - endTime) < SESSION_GAP_TRESHOLD) {
        //nothing to submit or previous session is continued
        //nothing to do - proceed with saved in UD values of counters
    } else {
        cocos2d::UserDefault *ud = cocos2d::UserDefault::getInstance();
        const int interstitialsCount = ud->getIntegerForKey(INTERSTITIAL_COUNTER_KEY.c_str(), 0);
        const int rewardedVideosCount = ud->getIntegerForKey(REWARDED_VIDEO_COUNTER_KEY.c_str(), 0);
        Analytics::getInstance()->sendEvent("interstitials_shown_p_s", {{"count", ::StringUtils::toString(interstitialsCount)}});
        Analytics::getInstance()->sendEvent("rewarded_videos_shown_p_s", {{"count", ::StringUtils::toString(rewardedVideosCount)}});
        ud->setIntegerForKey(INTERSTITIAL_COUNTER_KEY.c_str(), 0);
        ud->setIntegerForKey(REWARDED_VIDEO_COUNTER_KEY.c_str(), 0);
    }
}

void AdsSessionCounter::onAppResignActive() {
    //increment saved counters by local values
    cocos2d::UserDefault *ud = cocos2d::UserDefault::getInstance();
    const int interstitialsCount = ud->getIntegerForKey(INTERSTITIAL_COUNTER_KEY.c_str(), 0);
    const int rewardedVideosCount = ud->getIntegerForKey(REWARDED_VIDEO_COUNTER_KEY.c_str(), 0);
    ud->setIntegerForKey(INTERSTITIAL_COUNTER_KEY.c_str(), interstitialsCount + countShownInterstitial);
    countShownInterstitial = 0;
    ud->setIntegerForKey(REWARDED_VIDEO_COUNTER_KEY.c_str(), rewardedVideosCount + countShownRewardedVideo);
    countShownRewardedVideo = 0;
    
    //set session end time
    const time_t currentTime = time(nullptr);
    ud->setStringForKey(END_TIME_KEY.c_str(), ::StringUtils::toString(currentTime));
}
