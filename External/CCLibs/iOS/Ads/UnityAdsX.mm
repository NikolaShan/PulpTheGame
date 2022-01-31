//
//  UnityAdsX.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 01/08/15.
//
//

#import <UnityAds/UnityAds.h>
#import "ObjcBridgingSupport.h"
#import "UnityAdsMultiplexerDelegate.h"

#include "UnityAdsX.h"

@interface IOSUnityAdsXDelegate : NSObject<UnityAdsDelegate>

@property (assign, nonatomic) std::function<void(bool success)> completion;
@property (assign, nonatomic) Advertise::AppearListener *appearListener;
@property (assign, nonatomic) bool wasSkipped;

@end

@implementation IOSUnityAdsXDelegate

- (instancetype)init {
    self = [super init];
    if (self) {
        [[UnityAdsMultiplexerDelegate sharedInstance] addDelegate:self];
    }
    return self;
}

- (void)dealloc {
    [[UnityAdsMultiplexerDelegate sharedInstance] removeDelegate:self];
}

- (void)unityAdsVideoCompleted:(NSString *)rewardItemKey skipped:(BOOL)skipped {
    self.wasSkipped = skipped;
}

- (void)unityAdsWillShow {
    if (_appearListener) {
        _appearListener->adWillShow();
    }
}

- (void)unityAdsDidHide {
    if (_appearListener) {
        _appearListener->adWillHide(true);
    }
    if (_completion) {
        _completion(!self.wasSkipped);
        _completion = nullptr;
    }
}

@end

class IOSUnityAdsX: public Advertise::UnityAdsX {
private:
    IOSUnityAdsXDelegate *iosDelegate;
    
public:
    virtual void start(const std::string &gameId) override {
        NSString *nsGameId = NSStringFromStdString(gameId);
        UIViewController *rootVC = [[[[UIApplication sharedApplication] delegate] window] rootViewController];
        [[UnityAds sharedInstance] startWithGameId:nsGameId andViewController:rootVC];
        
        iosDelegate = [[IOSUnityAdsXDelegate alloc] init];
        iosDelegate.appearListener = appearListener;
    }
    
    virtual bool canShowRewardedVideo() override {
        NSString *zoneId = @"rewardedVideoZone";
        return [[UnityAds sharedInstance] canShowZone:zoneId];
    }
    
    virtual void showRewardedVideo(std::function<void(bool success)> completion) override {
        iosDelegate.completion = completion;
        NSString *zoneId = @"rewardedVideoZone";
        if ([[UnityAds sharedInstance] canShowZone:zoneId]) {
            [[UnityAds sharedInstance] setZone:zoneId];
            [[UnityAds sharedInstance] show];
        }
    }
};

namespace Advertise {
    std::shared_ptr<UnityAdsX> UnityAdsX::getInstance() {
        static auto instance = std::make_shared<IOSUnityAdsX>();
        return instance;
    }
}
