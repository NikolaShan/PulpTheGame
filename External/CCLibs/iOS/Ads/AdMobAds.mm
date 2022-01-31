//
//  AdMobAds.cpp
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 01/05/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <GoogleMobileAds/GADBannerView.h>
#import <GoogleMobileAds/GADInterstitial.h>
#import <GoogleMobileAds/GADRewardBasedVideoAd.h>
#import <GoogleMobileAds/GADRewardBasedVideoAdDelegate.h>
#import <GoogleMobileAds/GADMobileAds.h>
#import "ObjcBridgingSupport.h"

#include "AdMobAds.h"

using namespace Advertise;

void AdMobManager::start(const std::string &appId) {
    [GADMobileAds configureWithApplicationID:NSStringFromStdString(appId)];
}

#pragma mark -
#pragma mark Interstitial

typedef void(^AdMobInterstitialCallback)(void);

@interface IOSAdMobInterstitialHolder : NSObject <GADInterstitialDelegate>

@property (strong) AdMobInterstitialCallback failCallback;
@property (strong) AdMobInterstitialCallback loadCallback;
@property (nonatomic) Advertise::AppearListener *appearListener;
@property (strong, nonatomic) GADInterstitial *interstitial;

@property (strong, nonatomic) IOSAdMobInterstitialHolder *strongSelf;

@end

@implementation IOSAdMobInterstitialHolder

- (instancetype)initWithAdUnitId:(NSString *)adUnitId {
    self = [super init];
    if (self) {
        _interstitial = [[GADInterstitial alloc] initWithAdUnitID:adUnitId];
        _interstitial.delegate = self;
        
        GADRequest *request = [GADRequest request];
        [_interstitial loadRequest:request];
    }
    return self;
}

- (void)interstitial:(GADInterstitial *)ad didFailToReceiveAdWithError:(GADRequestError *)error {
    if (self.failCallback) {
        self.failCallback();
    }
}

- (void)interstitialDidReceiveAd:(GADInterstitial *)ad {
    if (self.loadCallback) {
        self.loadCallback();
    }
}

- (void)interstitialDidDismissScreen:(GADInterstitial *)ad {
    if (_appearListener) {
        _appearListener->adWillHide(false);
        _appearListener = nullptr;
    }
    self.strongSelf = nil;
}

- (void)presentFromViewController:(UIViewController *)vc {
    if (_appearListener) {
        _appearListener->adWillShow();
        self.strongSelf = self;
    }
    [self.interstitial presentFromRootViewController:vc];
}

@end

class IOSAdMobInterstitial: public Advertise::AdMobInterstitial {
private:
    bool presentWhenLoaded = false;
    IOSAdMobInterstitialHolder *advertise;
    
public:
    IOSAdMobInterstitial(const std::string &identifier): AdMobInterstitial(identifier) {
        advertise = nil;
    }
    
    virtual ~IOSAdMobInterstitial() {
        if (advertise) {
            advertise.interstitial.delegate = nil;
            advertise.interstitial = nil;
            advertise.loadCallback = nil;
            advertise.failCallback = nil;
        }
    }
    
    virtual void load() override {
        if (!advertise) {
            NSString *adUnitId = [NSString stringWithUTF8String:identifier.c_str()];
            advertise = [[IOSAdMobInterstitialHolder alloc] initWithAdUnitId:adUnitId];
            advertise.failCallback = ^{
                advertise = nil;
            };
            advertise.loadCallback = ^{
                this->didLoad();
            };
        }
    }
    
    virtual void present() override {
        presentWhenLoaded = false;
        if (isLoaded()) {
            advertise.appearListener = AdsManager::getInstance()->getAppearListener();
            UIViewController *rootVC = [[[[UIApplication sharedApplication] delegate] window] rootViewController];
            [advertise presentFromViewController:rootVC];
            advertise.failCallback = nil;
            advertise.loadCallback = nil;
            advertise = nil;
        } else {
            if (!advertise) {
                load();
                advertise.appearListener = AdsManager::getInstance()->getAppearListener();
            }
            presentWhenLoaded = true;
        }
    }
    
    virtual Status getCurrentStatus() const override {
        if (advertise == nil) {
            return Status::NONE;
        } else if (isLoaded()) {
            return Status::LOADED;
        } else {
            return Status::LOADING;
        }
    }
    
    bool isLoaded() const {
        return [advertise.interstitial isReady];
    }
    
    void didLoad() {
        if (presentWhenLoaded) {
            present();
        }
    }
};

std::shared_ptr<Advertise::Interstitial> Advertise::AdMobInterstitial::create(const std::string &identifier) {
    return std::dynamic_pointer_cast<Advertise::Interstitial>(std::make_shared<IOSAdMobInterstitial>(identifier));
}


#pragma mark -
#pragma mark Banner

typedef void(^AdMobBannerCallback)(void);

@interface IOSAdMobBannerDelegate : NSObject <GADBannerViewDelegate>

@property (strong, nonatomic) AdMobBannerCallback loadCallback;
@property (strong, nonatomic) AdMobBannerCallback failCallback;

@end

@implementation IOSAdMobBannerDelegate

- (void)adViewDidReceiveAd:(GADBannerView *)view {
    if (self.loadCallback) {
        self.loadCallback();
    }
}

- (void)adView:(GADBannerView *)view didFailToReceiveAdWithError:(GADRequestError *)error {
    if (self.failCallback) {
        self.failCallback();
    }
}

@end


class IOSAdMobBanner: public Advertise::AdMobBanner {
private:
    GADBannerView *bannerView;
    IOSAdMobBannerDelegate *delegateHandler;
    
private:
    void loadIfNecessary() {
        if (bannerView) {
            return;
        }
        UIViewController *vc = [[[UIApplication sharedApplication] delegate] window].rootViewController;
        UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
        if ( UIInterfaceOrientationIsPortrait(orientation) ) {
            bannerView = [[GADBannerView alloc] initWithAdSize:kGADAdSizeSmartBannerPortrait];
        } else {
            bannerView = [[GADBannerView alloc] initWithAdSize:kGADAdSizeSmartBannerLandscape];
        }
        
        [bannerView setDelegate:delegateHandler];
        [bannerView setAdUnitID:[NSString stringWithUTF8String:identifier.c_str()]];
        [bannerView setRootViewController:vc];
        [bannerView loadRequest:[GADRequest request]];
    }
    
public:
    
    IOSAdMobBanner(const std::string &identifier): AdMobBanner(identifier) {
        bannerView = nil;
        delegateHandler = [[IOSAdMobBannerDelegate alloc] init];
        delegateHandler.loadCallback = ^() {
            didLoadAds();
        };
        delegateHandler.failCallback = ^() {
            didFailToLoadAds();
        };
    }
    
    virtual ~IOSAdMobBanner() {
        bannerView.delegate = nil;
        delegateHandler.loadCallback = nil;
        delegateHandler.failCallback = nil;
        
        GADBannerView *gadBannerView = bannerView;
        dispatch_block_t block = ^{
            [gadBannerView removeFromSuperview];
        };
        if ([NSThread isMainThread]) {
            block();
        } else {
            dispatch_async(dispatch_get_main_queue(), block);
        }
    }
    
    virtual void remove() override {
        [bannerView removeFromSuperview];
    }
    
    virtual void present(float xPercent, float yPercent, float yAnchor) override {
        loadIfNecessary();
        
        UIViewController *vc = [[[UIApplication sharedApplication] delegate] window].rootViewController;
        bannerView.layer.anchorPoint = CGPointMake(0.0f, 1.0f - yAnchor);
        bannerView.autoresizingMask = UIViewAutoresizingNone;
        
        CGFloat xPos = 0;
        CGFloat yPos = (1.0f - yPercent) * vc.view.frame.size.height;
        
        [bannerView setCenter:CGPointMake(xPos, yPos)];
        if (!bannerView.superview) {
            [vc.view addSubview:bannerView];
        }
    }
    
    void didLoadAds() {
    }
    
    void didFailToLoadAds() {
        [bannerView removeFromSuperview];
        bannerView = nil;
    }
};

std::shared_ptr<Advertise::Banner> Advertise::AdMobBanner::create(const std::string &identifier) {
    return std::dynamic_pointer_cast<Advertise::Banner>(std::make_shared<IOSAdMobBanner>(identifier));
}


# pragma mark Rewarded Video

class AdMobRewardedVideoIOS;

@interface AdMobRewardedVideoDelegate : NSObject<GADRewardBasedVideoAdDelegate>
- (instancetype)initWithNative:(AdMobRewardedVideoIOS *)native;
@end


class AdMobRewardedVideoIOS: public AdMobRewardedVideo {
private:
    std::string adUnitId;
    AdMobRewardedVideoDelegate *delegate;
    std::function<void(bool success)> completion;
    std::function<void(bool success)> loadCompletion;

    bool presentWhenReady = false;
    
public:
    virtual void setAdUnitId(const std::string &adUnitId) override {
        this->adUnitId = adUnitId;
        delegate = [[AdMobRewardedVideoDelegate alloc] initWithNative:this];
        [GADRewardBasedVideoAd sharedInstance].delegate = delegate;
    }
    
    virtual void load() override {
        [[GADRewardBasedVideoAd sharedInstance] loadRequest:[GADRequest request]
                                               withAdUnitID:NSStringFromStdString(adUnitId)];
    }
    
    virtual void load(std::function<void(bool success)> completion) override {
        loadCompletion = completion;
        load();
    }
    
    virtual void present(std::function<void(bool success)> completion) override {
        this->completion = completion;
        if ([[GADRewardBasedVideoAd sharedInstance] isReady]) {
            present();
        } else {
            presentWhenReady = true;
        }
    }
    
    virtual Status getCurrentStatus() const override {
        if ([[GADRewardBasedVideoAd sharedInstance] isReady]) {
            return Status::LOADED;
        }
        return Status::NONE;
    }
    
    void present() {
        presentWhenReady = false;
        UIViewController *vc = [[[UIApplication sharedApplication] delegate] window].rootViewController;
        [[GADRewardBasedVideoAd sharedInstance] presentFromRootViewController:vc];
    }
    
    void adDidLoad() {
        if (loadCompletion) {
            loadCompletion(true);
            loadCompletion = nullptr;
        }
        if (presentWhenReady) {
            present();
        }
    }
    
    void adDidFailToLoad() {
        if (loadCompletion) {
            loadCompletion(false);
            loadCompletion = nullptr;
        }
        
        presentWhenReady = false;
        if (completion) {
            completion(false);
            completion = nullptr;
        }
    }
    
    void adWillShow() {
        AdsManager::getInstance()->getAppearListener()->adWillShow();
    }
    
    void adDidClose() {
        AdsManager::getInstance()->getAppearListener()->adWillHide(true);
        if (completion) {
            completion(false);
            completion = nullptr;
        }
        load();
    }
    
    void didReward() {
        if (completion) {
            completion(true);
            completion = nullptr;
        }
    }
};


AdMobRewardedVideo *AdMobRewardedVideo::getInstance() {
    static auto ret = new AdMobRewardedVideoIOS();
    return ret;
}

@implementation AdMobRewardedVideoDelegate {
    AdMobRewardedVideoIOS *_native;
}

- (instancetype)initWithNative:(AdMobRewardedVideoIOS *)native {
    self = [super init];
    if (self) {
        _native = native;
    }
    return self;
}

- (void)rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd
   didRewardUserWithReward:(GADAdReward *)reward {
    _native->didReward();
}

- (void)rewardBasedVideoAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd
    didFailToLoadWithError:(NSError *)error {
    _native->adDidFailToLoad();
}

- (void)rewardBasedVideoAdDidReceiveAd:(GADRewardBasedVideoAd *)rewardBasedVideoAd {
    _native->adDidLoad();
}

- (void)rewardBasedVideoAdDidStartPlaying:(GADRewardBasedVideoAd *)rewardBasedVideoAd {
    _native->adWillShow();
}

- (void)rewardBasedVideoAdDidClose:(GADRewardBasedVideoAd *)rewardBasedVideoAd {
    _native->adDidClose();
}

@end
