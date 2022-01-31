//
//  Chartboost.cpp
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 02/05/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Chartboost/Chartboost.h>

#import "ObjcBridgingSupport.h"
#import "ChartboostMultiplexerDelegate.h"

#include "ChartboostX.h"


#pragma mark - ObjC to Cpp chartboost delegate

@interface IOSChartboostDelegate : NSObject <ChartboostDelegate>

@property (nonatomic, assign) Advertise::ChartboostX::Delegate *cppDelegate;
@property (nonatomic, assign) Advertise::AppearListener *appearListener;
@property (nonatomic, assign) std::function<void(bool success)> rewardedVideoCompletion;
@property (nonatomic) BOOL completedRewardedVideo;

@end

@implementation IOSChartboostDelegate

- (instancetype)init {
    if (self = [super init]) {
        [[ChartboostMultiplexerDelegate sharedInstance] addDelegate:self];
    }
    return self;
}

- (void)dealloc {
    [[ChartboostMultiplexerDelegate sharedInstance] removeDelegate:self];
}

// Interstitials

- (void)didCacheInterstitial:(CBLocation)location {
    if (_cppDelegate) {
        _cppDelegate->didLoadInterstitial([location UTF8String]);
    }
}

- (void)didFailToLoadInterstitial:(CBLocation)location
                        withError:(CBLoadError)error {
    if (_cppDelegate) {
        _cppDelegate->didFailToLoadInterstitial([location UTF8String], error);
    }
}

- (void)didDismissInterstitial:(CBLocation)location {
    if (_appearListener) {
        _appearListener->adWillHide(false);
    }
}

- (void)didDisplayInterstitial:(CBLocation)location {
    if (_appearListener) {
        _appearListener->adWillShow();
    }
}


// More Apps

- (void)didCacheMoreApps:(CBLocation)location {
    if (_cppDelegate) {
        _cppDelegate->didLoadMoreApps([location UTF8String]);
    }
}

- (void)didFailToLoadMoreApps:(CBLocation)location
                    withError:(CBLoadError)error {
    if (_cppDelegate) {
        _cppDelegate->didFailToLoadMoreApps([location UTF8String], error);
    }
}


// Rewarded videos

- (void)didCacheRewardedVideo:(CBLocation)location {
    if (_cppDelegate) {
        _cppDelegate->didLoadRewardedVideo([location UTF8String]);
    }
}

- (void)didFailToLoadRewardedVideo:(CBLocation)location
                         withError:(CBLoadError)error {
    if (_cppDelegate) {
        _cppDelegate->didFailToLoadRewardedVideo([location UTF8String], error);
    }
}

- (void)didCompleteRewardedVideo:(CBLocation)location
                      withReward:(int)reward {
    if (_cppDelegate) {
        _cppDelegate->didCompleteRewardedVideo([location UTF8String]);
    }
    self.completedRewardedVideo = reward > 0;
}

- (void)didDismissRewardedVideo:(CBLocation)location {
    if (_appearListener) {
        _appearListener->adWillHide(true);
    }
    if (_rewardedVideoCompletion) {
        _rewardedVideoCompletion(_completedRewardedVideo);
    }
    _rewardedVideoCompletion = nullptr;
    _completedRewardedVideo = NO;
}

- (void)didDisplayRewardedVideo:(CBLocation)location {
    if (_appearListener) {
        _appearListener->adWillShow();
    }
}

@end


#pragma mark -
#pragma mark Implementation

class IOSChartboostX: public Advertise::ChartboostX {
private:
    IOSChartboostDelegate *objcDelegate;
    
public:
    IOSChartboostX() {
        objcDelegate = [[IOSChartboostDelegate alloc] init];
        objcDelegate.cppDelegate = nullptr;
        objcDelegate.appearListener = appearListener;
    }
    
    virtual ~IOSChartboostX() {
        objcDelegate.cppDelegate = nil;
        objcDelegate.appearListener = nullptr;
    }
    
public:
    virtual void setDelegate(Delegate *delegate) override {
        ChartboostX::setDelegate(delegate);
        objcDelegate.cppDelegate = delegate;
    }
    
    virtual void startSession(const std::string &appId, const std::string &appSignature) override {
        NSString *nsAppId = NSStringFromStdString(appId);
        NSString *nsAppSignature = NSStringFromStdString(appSignature);
        [Chartboost startWithAppId:nsAppId appSignature:nsAppSignature delegate:[ChartboostMultiplexerDelegate sharedInstance]];
    }
    
    virtual bool hasInterstitial(const Location &location) const override {
        return [Chartboost hasInterstitial: NSStringFromStdString(location)];
    }
    
    virtual void loadInterstitial(const Location &location) override {
        [Chartboost cacheInterstitial: NSStringFromStdString(location)];
    }
    
    virtual void presentInterstitial(const Location &location) override {
        [Chartboost showInterstitial: NSStringFromStdString(location)];
    }
    
    virtual bool hasRewardedVideo(const Location &location) const override {
        return [Chartboost hasRewardedVideo: NSStringFromStdString(location)];
    }
    
    virtual void loadRewardedVideo(const Location &location) override {
        [Chartboost cacheRewardedVideo: NSStringFromStdString(location)];
    }
    
    virtual void presentRewardedVideo(const Location &location, std::function<void(bool success)> completion) override {
        objcDelegate.rewardedVideoCompletion = completion;
        [Chartboost showRewardedVideo: NSStringFromStdString(location)];
    }
    
/*    virtual bool hasMoreApps(const Location &location) const override {
        return [Chartboost hasMoreApps: NSStringFromStdString(location)];
    }
    
    virtual void loadMoreApps(const Location &location) override {
        [Chartboost cacheMoreApps: NSStringFromStdString(location)];
    }
    
    virtual void presentMoreApps(const Location &location) override {
        [Chartboost showMoreApps: NSStringFromStdString(location)];
    }*/
};

Advertise::ChartboostX *Advertise::ChartboostX::getInstance() {
    static IOSChartboostX *instance = new IOSChartboostX();
    return instance;
}
