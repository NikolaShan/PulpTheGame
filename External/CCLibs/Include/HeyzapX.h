//
//  HeyzapX.h
//  Pulp
//
//  Created by Mikhail Shulepov on 08/09/15.
//
//

#pragma once

#include <string>
#include <assert.h>

#include "Advertise.h"

namespace Advertise {
    
    class HeyzapX: public AdSystem {
    public:
        static const std::string DEFAULT_TAG;
        
        static HeyzapX *getInstance();
        virtual void startSession(const std::string &publisherId) = 0;
        
        virtual void loadInterstitial(const std::string &tag = HeyzapX::DEFAULT_TAG) = 0;
        virtual void presentInterstitial(const std::string &tag) = 0;
        void presentInterstitial() {
            presentInterstitial(HeyzapX::DEFAULT_TAG);
        }
        virtual Status getInterstitialStatus(const std::string &tag = HeyzapX::DEFAULT_TAG) = 0;
        
        virtual void loadRewardedVideo(const std::string &tag, std::function<void(bool)> completion) = 0;
        void loadRewardedVideo(std::function<void(bool)> completion) {
            loadRewardedVideo(HeyzapX::DEFAULT_TAG, completion);
        }
        virtual void presentRewardedVideo(const std::string &tag, std::function<void(bool success)> completion) = 0;
        void presentRewardedVideo(std::function<void(bool success)> completion) {
            presentRewardedVideo(HeyzapX::DEFAULT_TAG, completion);
        }
        virtual Status getRewardedVideoStatus(const std::string &tag = HeyzapX::DEFAULT_TAG) const = 0;
        
        AppearListener *getAppearListener() const {
            return appearListener;
        }
        
    protected:
        HeyzapX() { }
        virtual ~HeyzapX() { }
    };
    
    class HeyzapBanner: public Advertise::Banner {
    public:
        static std::shared_ptr<Banner> create(const std::string &tag = HeyzapX::DEFAULT_TAG);
        
        HeyzapBanner(const std::string &pTag): tag(pTag)  {
            
        }
        
        virtual void remove() = 0;
        virtual void present(float xPercent, float yPercent, float yAnchor) = 0;
        
    protected:
        std::string tag;
    };
    
    class HeyzapInterstitial: public Advertise::Interstitial {
    public:
        static std::shared_ptr<Interstitial> create(const std::string &tag = HeyzapX::DEFAULT_TAG) {
            return std::make_shared<HeyzapInterstitial>(tag);
        }
        
        HeyzapInterstitial(const std::string &pTag): tag(pTag)  {
            
        }
        
        virtual ~HeyzapInterstitial() {}
        
        virtual void load() override {
            HeyzapX::getInstance()->loadInterstitial(tag);
        }
        
        virtual void present() override {
            HeyzapX::getInstance()->presentInterstitial(tag);
        }
        
        virtual Status getCurrentStatus() const override {
            return HeyzapX::getInstance()->getInterstitialStatus(tag);
        }
        
    protected:
        std::string tag;
    };
    
    
    class HeyzapRewardedVideo: public RewardedVideo {
    public:
        static std::shared_ptr<HeyzapRewardedVideo> create(const std::string &tag = HeyzapX::DEFAULT_TAG) {
            return std::make_shared<HeyzapRewardedVideo>(tag);
        }
        HeyzapRewardedVideo(const std::string &pTag) : tag(pTag) {
            
        }
        
        virtual ~HeyzapRewardedVideo() {}
        
        virtual void load() override {
            HeyzapX::getInstance()->loadRewardedVideo(tag, nullptr);
        }
        
        virtual void present(std::function<void(bool success)> completion) override {
            HeyzapX::getInstance()->presentRewardedVideo(tag, completion);
        }
        
        virtual Status getCurrentStatus() const override {
            return HeyzapX::getInstance()->getRewardedVideoStatus(tag);
        }
        
    private:
        std::string tag;
    };
    
}

