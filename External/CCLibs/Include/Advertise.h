//
//  Advertise.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 01/05/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#ifndef CocosFrameworksBinding_Advertise_h
#define CocosFrameworksBinding_Advertise_h

#include <memory>
#include <time.h>
#include <functional>

class BaseAppSettings;

namespace Advertise {
    //Fail status not required
    //If ads loading failed - Interstitial should return false Status::NONE
    enum class Status {
        NONE,
        LOADING,
        LOADED
    };
    
    class AppearListener {
    public:
        virtual ~AppearListener() {}
        virtual void adWillShow() = 0;
        virtual void adWillHide(bool rewarded) = 0;
    };
       
    class Interstitial {
    public:
        virtual ~Interstitial() {}
        virtual void load() = 0;
        virtual void present() = 0;
        virtual Status getCurrentStatus() const = 0;
    };
    
    class Banner {
    public:
        virtual ~Banner() {}

        virtual void remove() = 0;
        
        //x anchor always = 0.5
        //to display at bottom: 0.5 , 0, 0
        //to diaplay in center: 0.5, 0.5, 0.5
        //to display on top: 0.5, 1, 1
        virtual void present(float xPercent, float yPercent, float yAnchor) = 0;
    };
    
    class RewardedVideo {
    public:
        virtual ~RewardedVideo() {}
        
        virtual void load() = 0;
        virtual void present(std::function<void(bool success)> completion) = 0;
        virtual Status getCurrentStatus() const = 0;
    };
    
    class InterstitialScheduler {
    public:
        static std::shared_ptr<InterstitialScheduler> getDefaultInstance() {
            static auto instance = std::make_shared<InterstitialScheduler>();
            return instance;
        }
        
        InterstitialScheduler();
        
        //
        void setIntersititalProvider(std::shared_ptr<Interstitial> interstitial) {
            this->interstitial = interstitial;
        }
        
        //40sec by default
        void setMinIntervalBetweenAds(long intervalInSec) {
            minIntervalBetweenAds = intervalInSec;
        }
        
        //40 sec by default
        void setMinIntervalFromLaunch(long timeInSec) {
            minIntervalFromLaunch = timeInSec;
        }
        
        //5 by default
        void setRequiredEventsCount(unsigned count) {
            requiredEventsCount = count;
        }
        
        void setupWithAppSettings(std::shared_ptr<BaseAppSettings> settings) {
            appSettings = settings;
        }
        
        long getMinIntervalBetweenAds() const;
        long getMinIntervalFromLaunch() const;
        long getRequiredEventsCount() const;
        
        bool fireEvent();
        void resetTimeAndEvents();
        
        int getCountShown() const { return timesShown; }
        void resetCountShown() {
            timesShown = 0;
        }
        
    private:
        //
        std::shared_ptr<Interstitial> interstitial;
        
        //settings
        time_t minIntervalBetweenAds = 40;//sec
        time_t minIntervalFromLaunch = 40;//sec
        int requiredEventsCount = 5;
        std::shared_ptr<BaseAppSettings> appSettings;
        
        //runtime
        time_t launchTime;
        time_t lastTimeAdsShown = 0;
        int eventsCount = 0;
        int timesShown = 0;
    };
    
    class DefaultAppearListener: public Advertise::AppearListener {
    public:
        static DefaultAppearListener *getInstance() {
            static DefaultAppearListener *instance = new DefaultAppearListener;
            return instance;
        }
        
        void setResetOnRewardedVideo(bool reset) {
            resetOnRewardedVideo = reset;
        }
        
        virtual void adWillShow() override;
        virtual void adWillHide(bool rewarded) override;
        
    private:
        float backgroundMusicVolume = 0.0f;
        bool resetOnRewardedVideo = true;
    };
    
    class AdSystem {
    public:
        
    protected:
    };
}

class AdsManager {
public:
    using InterstitialSchedulerPtr = std::shared_ptr<Advertise::InterstitialScheduler>;
    
    static std::shared_ptr<AdsManager> getInstance() {
        static auto instance = std::make_shared<AdsManager>();
        return instance;
    }
    
    AdsManager(): appearListener(Advertise::DefaultAppearListener::getInstance()) {}
    
    bool isAdsEnabled() const;
    void setAdsEnabled(bool enabled);
    
    void setBannerProvider(std::shared_ptr<Advertise::Banner> banner);
    void setInterstitialScheduler(std::shared_ptr<Advertise::InterstitialScheduler> scheduler);
    
    void removeBanner();
    void presentBanner(float xPercent, float yPercent, float yAnchor);
    std::shared_ptr<Advertise::Banner> getBanner() { return banner; }
    
    bool fireEvent();
    void resetTimeAndEvents();
    
    int getInterstitialCountShown() const {
        if (interstitialScheduler) {
            return interstitialScheduler->getCountShown();
        }
        return 0;
    }
    
    void resetInterstitialCountShown() {
        if (interstitialScheduler) {
            interstitialScheduler->resetCountShown();
        }
    }
    
    Advertise::AppearListener *getAppearListener() const {
        return appearListener;
    }
    
    void setAppearListener(Advertise::AppearListener *listener) {
        appearListener = listener;
    }
   
private:
    std::shared_ptr<Advertise::Banner> banner;
    InterstitialSchedulerPtr interstitialScheduler;
    Advertise::AppearListener *appearListener;

};

#endif
