//
//  Appodeal.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 04/07/15.
//
//

#import <Appodeal/Appodeal.h>
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import "VKSdk.h"
#import "ObjcBridgingSupport.h"

#include "Appodeal.h"

@interface IOSAppodealDelegate : NSObject<AppodealInterstitialDelegate, AppodealVideoDelegate, AppodealRewardedVideoDelegate>

@property (nonatomic) Advertise::AppearListener *appearListener;
@property (nonatomic) std::function<void(bool success)> rewardedVideoCompletion;

@end

@implementation IOSAppodealDelegate {
    
}

- (instancetype)init {
    self = [super init];
    if (self) {
        
    }
    return self;
}

#pragma mark Interstitial

- (void)interstitialDidLoadAd {
    NSLog(@"Appodeal: Interstitial did load");
}

- (void)interstitialDidFailToLoadAd {
    NSLog(@"Appodeal: Interstitial did fail to load");
}

- (void)interstitialDidDismiss {
    if (_appearListener) {
        _appearListener->adWillHide(false);
    }
}

- (void)interstitialWillPresent {
    if (_appearListener) {
        _appearListener->adWillShow();
    }
}

#pragma mark Video

- (void)videoDidLoadAd {
    NSLog(@"Appodeal: video did load");
}

- (void)videoDidFailToLoadAd {
    NSLog(@"Appodeal: video did fail to load");
}

- (void)videoDidPresent {
    if (_appearListener) {
        _appearListener->adWillShow();
    }
}

- (void)videoWillDismiss {
    if (_appearListener) {
        _appearListener->adWillHide(false);
    }
}

- (void)videoDidFinish {
    
}

#pragma mark - Rewarded videos

- (void)rewardedVideoDidLoadAd {
    //rewarded video ready
}

- (void)rewardedVideoDidFailToLoadAd {
    NSLog(@"Appodeal: rewarded video did fail to load");
    /*if (_rewardedVideoCompletion && !rewardedVideoPresented) {
     _rewardedVideoCompletion(false);
     _rewardedVideoCompletion = nullptr;
     }*/
}

- (void)rewardedVideoDidPresent {
    if (_appearListener) {
        _appearListener->adWillShow();
    }
}

- (void)rewardedVideoWillDismiss {
    NSLog(@"Rewarded video will dismiss");
    if (_appearListener) {
        _appearListener->adWillHide(true);
    }
    if (_rewardedVideoCompletion) {
        _rewardedVideoCompletion(true);
        _rewardedVideoCompletion = nullptr;
    }
}

- (void)rewardedVideoDidFinish:(NSUInteger)rewardAmount name:(NSString *)rewardName {
    NSLog(@"Rewarded video did finish");
    if (_rewardedVideoCompletion) {
        _rewardedVideoCompletion(true);
        _rewardedVideoCompletion = nullptr;
    }
}

@end

class IOSAppodeal: public Advertise::AppodealAds {
private:
    IOSAppodealDelegate *iosDelegate;
    
public:
    virtual void startWithAppKey(const std::string &key) override {
        iosDelegate = [[IOSAppodealDelegate alloc] init];
        iosDelegate.appearListener = appearListener;
        
        //[Appodeal setAutocache:NO types:AppodealAdTypeAll];
        [Appodeal setAutocache:NO types:AppodealAdTypeMREC];
        [Appodeal setAutocache:NO types:AppodealAdTypeNativeAd];
        
        [Appodeal disableLocationPermissionCheck];
        [Appodeal initializeWithApiKey:NSStringFromStdString(key) types: AppodealAdTypeAll];
        [Appodeal setInterstitialDelegate:iosDelegate];
        [Appodeal setVideoDelegate:iosDelegate];
        [Appodeal setRewardedVideoDelegate:iosDelegate];
        
        CCLOG("Autocache banners: %s", [Appodeal isAutocacheEnabled:AppodealAdTypeBanner] ? "yes" : "no");
        CCLOG("Autocache interstitials: %s", [Appodeal isAutocacheEnabled:AppodealAdTypeInterstitial] ? "yes" : "no");
        CCLOG("Autocache videos: %s", [Appodeal isAutocacheEnabled:AppodealAdTypeSkippableVideo] ? "yes" : "no");
        CCLOG("Autocache rewarded videos: %s", [Appodeal isAutocacheEnabled:AppodealAdTypeRewardedVideo] ? "yes" : "no");
        
        //setup with vk
        VKAccessToken *vkToken = [VKSdk accessToken];
        if (vkToken && vkToken.userId) {
            [Appodeal setUserVkId:vkToken.userId];
        }
        
        //setup with fb
        FBSDKAccessToken *fbToken = [FBSDKAccessToken currentAccessToken];
        if (fbToken && fbToken.userID) {
            [Appodeal setUserFacebookId:fbToken.userID];
        }
    }
    
    virtual void removeBanner() override {
        //fix: banner removed in destructor. Destructor may be called not on main thread
        dispatch_block_t block = ^{
            [Appodeal hideBanner];
        };
        if ([NSThread isMainThread]) {
            block();
        } else {
            dispatch_async(dispatch_get_main_queue(), block);
        }
    }
    
    virtual void presentBanner(float xPercent, float yPercent, float yAnchor) override {
        UIViewController *topVC = [[[UIApplication sharedApplication] delegate] window].rootViewController;
        if (yPercent < 0.2f) {
            [Appodeal showAd:AppodealShowStyleBannerBottom rootViewController:topVC];
        } else if (yPercent < 0.6f) {
            [Appodeal showAd:AppodealShowStyleBannerCenter rootViewController:topVC];
        } else {
            [Appodeal showAd:AppodealShowStyleBannerTop rootViewController:topVC];
        }
    }
    
    virtual void presentInterstitial() override {
        UIViewController *topVC = [[[UIApplication sharedApplication] delegate] window].rootViewController;
        [Appodeal showAd:AppodealShowStyleInterstitial rootViewController:topVC];
    }
    
    virtual void presentVideoInterstitial() override {
        UIViewController *topVC = [[[UIApplication sharedApplication] delegate] window].rootViewController;
        [Appodeal showAd:AppodealShowStyleSkippableVideo rootViewController:topVC];
    }
    
    virtual void presentInterstitialStaticOrVideo() override {
        UIViewController *topVC = [[[UIApplication sharedApplication] delegate] window].rootViewController;
        [Appodeal showAd:AppodealShowStyleVideoOrInterstitial rootViewController:topVC];
    }
    
    virtual void presentRewardedVideo(std::function<void(bool success)> completion) override {
        iosDelegate.rewardedVideoCompletion = completion;
        UIViewController *topVC = [[[UIApplication sharedApplication] delegate] window].rootViewController;
        [Appodeal showAd:AppodealShowStyleRewardedVideo rootViewController:topVC];
    }
    
    virtual bool hasCachedRewardedVideo() const override {
        return [Appodeal isReadyForShowWithStyle:AppodealShowStyleRewardedVideo];
    }
};

std::shared_ptr<Advertise::AppodealAds> Advertise::AppodealAds::getInstance() {
    static auto instance = std::dynamic_pointer_cast<AppodealAds>(std::make_shared<IOSAppodeal>());
    return instance;
}
