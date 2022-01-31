//
//  AppodealAds.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 04/07/15.
//
//

#ifndef CCLibs_AppodealAds_h
#define CCLibs_AppodealAds_h

#include <string>
#include <functional>
#include <memory>

#include "Advertise.h"
#include "Utils/BaseAppSettings.h"

namespace Advertise {
    
    class AppodealAds: public AdSystem {
    public:
        static std::shared_ptr<AppodealAds> getInstance();
        virtual ~AppodealAds() {}
        
        virtual void startWithAppKey(const std::string &key) = 0;
        
        virtual void removeBanner() = 0;
        virtual void presentBanner(float xPercent, float yPercent, float yAnchor) = 0;
        
        virtual void presentVideoInterstitial() = 0;
        virtual void presentInterstitial() = 0;
        virtual void presentInterstitialStaticOrVideo() = 0;
        virtual void presentRewardedVideo(std::function<void(bool success)> completion) = 0;
        
        virtual bool hasCachedRewardedVideo() const = 0;
        
        AppearListener *getAppearListener() const {
            return appearListener;
        }
        
    protected:
        AppodealAds() {}
    };
    
    class AppodealBanner: public Banner {
    public:
        static std::shared_ptr<Banner> create() {
            return std::make_shared<AppodealBanner>();
        }
        
        virtual ~AppodealBanner() {
            remove();
        }
        
        virtual void remove() override {
            AppodealAds::getInstance()->removeBanner();
        }
        
        virtual void present(float xPercent, float yPercent, float yAnchor) override {
            AppodealAds::getInstance()->presentBanner(xPercent, yPercent, yAnchor);
        }
    };
    
    class AppodealInterstitial: public Interstitial {
    public:
        static std::shared_ptr<Interstitial> create(std::shared_ptr<BaseAppSettings> appSettings) {
            return std::dynamic_pointer_cast<Interstitial>(std::make_shared<AppodealInterstitial>(appSettings));
        }
        
        //chance - 0..1
        //if chance < 0 - Appodeal itself will decide what to show
        //if chance >= 1 - always will be shown video ads
        AppodealInterstitial(std::shared_ptr<BaseAppSettings> pappSettings): appSettings(pappSettings) { }
        
        virtual void load() override {
            //nothing to do
        }
        
        virtual void present() override {
            const float fr = (rand() % 100) / 100.0f;
            auto appodeal = AppodealAds::getInstance();
            const float videoChance = appSettings ? appSettings->getFloat("appodeal/video_chance") : -1.0f;
            if (videoChance < 0) {
                appodeal->presentInterstitialStaticOrVideo();
            } else if (fr < videoChance) {
                appodeal->presentVideoInterstitial();
            } else {
                appodeal->presentInterstitial();
            }
        }
        
        virtual Status getCurrentStatus() const override {
            return Status::LOADED;
        }
        
    private:
        std::shared_ptr<BaseAppSettings> appSettings;
    };
}



#endif
