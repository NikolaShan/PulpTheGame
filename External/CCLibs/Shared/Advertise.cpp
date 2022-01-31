//
//  Advertise.cpp
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#include "Utils/PLAudioEngine.h"

#include "Advertise.h"
#include "base/CCUserDefault.h"
#include "base/CCConsole.h" 
#include "Utils/BaseAppSettings.h"

namespace Advertise {   
    InterstitialScheduler::InterstitialScheduler(): appSettings(nullptr) {
        launchTime = time(nullptr);
    }
    
    bool InterstitialScheduler::fireEvent() {
        eventsCount++;
        if (!interstitial.get()) {
            return false;
        }
        
        if (eventsCount > (getRequiredEventsCount() - 2)) {
            if (interstitial->getCurrentStatus() == Advertise::Status::NONE) {
                interstitial->load();
            }
        }
        
        if (eventsCount < getRequiredEventsCount()) {
            return false;
        }
        
        time_t currentTime = time(nullptr);
        if (currentTime - launchTime < getMinIntervalFromLaunch()) {
            return false;
        }
        if (currentTime - lastTimeAdsShown < getMinIntervalBetweenAds()) {
            return false;
        }
        if (interstitial->getCurrentStatus() != Advertise::Status::LOADED) {
            return false;
        }

        interstitial->present();
        timesShown += 1;
        eventsCount = 0;
        lastTimeAdsShown = currentTime;
        
        return true;
    }
    
    long InterstitialScheduler::getMinIntervalBetweenAds() const {
        double intervalIncrease = 5.0;
        if (appSettings) {
            intervalIncrease = appSettings->getFloat("ads_interval_increase_step");
        }
        const long additionalDelay = static_cast<long>(intervalIncrease * std::min(7, timesShown));
        if (appSettings) {
            return appSettings->getFloat("ads_min_interval_b_interstitials") + additionalDelay;
        }
        return minIntervalBetweenAds + additionalDelay;
    }
    
    long InterstitialScheduler::getMinIntervalFromLaunch() const {
        if (appSettings) {
            return appSettings->getFloat("ads_min_interval_from_app_launch");
        }
        return minIntervalFromLaunch;
    }
    
    long InterstitialScheduler::getRequiredEventsCount() const {
        if (appSettings) {
            float ret = appSettings->getInt("ads_interstitial_min_events_count");
            return ret;
        }
        return requiredEventsCount;
    }
    
    void InterstitialScheduler::resetTimeAndEvents() {
        lastTimeAdsShown = time(nullptr);
        eventsCount = 0;
    }
    
    void DefaultAppearListener::adWillShow() {
        backgroundMusicVolume = PLAudioEngine::getInstance()->getBackgroundMusicVolume();
        PLAudioEngine::getInstance()->setBackgroundMusicVolume(0.0f);
    }
    
    void DefaultAppearListener::adWillHide(bool rewarded) {
        PLAudioEngine::getInstance()->setBackgroundMusicVolume(backgroundMusicVolume);
        if (!rewarded || resetOnRewardedVideo) {
            AdsManager::getInstance()->resetTimeAndEvents();
        }
    }
}


static const std::string AdsEnabledKey = "FlittyThiry432";
static const int AdsDisabledValue = 121;

bool AdsManager::isAdsEnabled() const {
    return cocos2d::UserDefault::getInstance()->getIntegerForKey(AdsEnabledKey.c_str(), 254) != AdsDisabledValue;
}

void AdsManager::setAdsEnabled(bool enabled) {
    auto ud = cocos2d::UserDefault::getInstance();
    ud->setIntegerForKey(AdsEnabledKey.c_str(), enabled ? 554: AdsDisabledValue);
    if (!enabled) {
        removeBanner();
    }
}

void AdsManager::setBannerProvider(std::shared_ptr<Advertise::Banner> banner) {
    this->banner = banner;
}

void AdsManager::setInterstitialScheduler( std::shared_ptr<Advertise::InterstitialScheduler> scheduler) {
    interstitialScheduler = scheduler;
}

void AdsManager::removeBanner() {
    if (banner) {
        banner->remove();
    }
}

void AdsManager::presentBanner(float xPercent, float yPercent, float yAnchor) {
    if (banner && isAdsEnabled()) {
        banner->present(xPercent, yPercent, yAnchor);
    }
}

bool AdsManager::fireEvent() {
    if (!isAdsEnabled()) {
        return false;
    }
    if (!interstitialScheduler) {
        cocos2d::log("WARNING: No interstitial scheduler available!");
        return false;
    }
    return interstitialScheduler->fireEvent();
}

void AdsManager::resetTimeAndEvents() {
    if (interstitialScheduler) {
        interstitialScheduler->resetTimeAndEvents();
    }
}
