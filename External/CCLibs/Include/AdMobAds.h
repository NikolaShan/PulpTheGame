//
//  AdMobAds.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 01/05/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#ifndef __CocosFrameworksBinding__AdMobAds__
#define __CocosFrameworksBinding__AdMobAds__

#include <string>
#include <functional>
#include <memory>

#include "Advertise.h"

namespace Advertise {
    
class AdMobManager {
public:
    static void start(const std::string &appId);
};
    
class AdMobInterstitial: public Advertise::Interstitial {
public:
    static std::shared_ptr<Advertise::Interstitial> create(const std::string &identifier);
    virtual ~AdMobInterstitial() {}
    
protected:
    AdMobInterstitial(const std::string &pIdentifier)
    : identifier(pIdentifier) {}
    
    const std::string identifier;
};

class AdMobBanner: public Advertise::Banner {
public:
    static std::shared_ptr<Advertise::Banner> create(const std::string &identifier);
    virtual ~AdMobBanner() {}
    
protected:
    AdMobBanner(const std::string &pidentifier): identifier(pidentifier) {}
    
    const std::string identifier;
};

class AdMobRewardedVideo: public Advertise::RewardedVideo {
public:
    static AdMobRewardedVideo *getInstance();
    
    virtual void setAdUnitId(const std::string &adUnitId) = 0;
    virtual void load() = 0;
    virtual void present(std::function<void(bool success)> completion) = 0;
    virtual Status getCurrentStatus() const = 0;
    
    virtual void load(std::function<void(bool success)> completion) = 0;
};
    
}

#endif /* defined(__CocosFrameworksBinding__AdMobAds__) */
