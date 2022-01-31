//
//  ChartboostAdapter.m
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import "ChartboostAdapter.h"
#import "ChartboostMultiplexerDelegate.h"

@interface ChartboostAdapter()

@property (strong, nonatomic) NSString *location;

@end

@implementation ChartboostAdapter

@synthesize delegate;

- (instancetype)init {
    if (self = [super init]) {
        [[ChartboostMultiplexerDelegate sharedInstance] addDelegate:self];
    }
    return self;
}

- (void)dealloc {
    [[ChartboostMultiplexerDelegate sharedInstance] removeDelegate:self];
}


#pragma mark -
#pragma mark GADCustomEventInterstitial

- (void)requestInterstitialAdWithParameter:(NSString *)serverParameter
                                     label:(NSString *)serverLabel
                                   request:(GADCustomEventRequest *)request {
    NSString *location = CBLocationGameScreen;
    
    if (serverParameter != nil && serverParameter.length > 2) {
        NSArray *components = [serverParameter componentsSeparatedByString:@";"];
        for (NSString *component in components) {
            NSArray *keyValue = [component componentsSeparatedByString:@"="];
            if ([keyValue count] >= 2) {
                NSString *key = keyValue[0];
                NSString *value = keyValue[1];
                if ([key isEqualToString:@"location"]) {
                    location = value;
                }
            }
        }
    }
    
    self.location = location;
    if ([Chartboost hasInterstitial:location]) {
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self didCacheInterstitial:location];
        });
    } else {
        [Chartboost cacheInterstitial:location];
    }
}

- (void)presentFromRootViewController:(UIViewController *)rootViewController {
    if (!self.location) {
        self.location = CBLocationGameScreen;
    }
    [Chartboost showInterstitial:self.location];
}


#pragma mark -
#pragma mark Chartboost Delegate

- (void)didCacheInterstitial:(CBLocation)location {
    [self.delegate customEventInterstitialDidReceiveAd:self];
}

- (void)didFailToLoadInterstitial:(CBLocation)location withError:(CBLoadError)error {
    NSError *nserror = [NSError errorWithDomain:@"AdMob.ChartboostAdapter" code:error userInfo:nil];
    [self.delegate customEventInterstitial:self didFailAd:nserror];
}

- (void)didDismissInterstitial:(CBLocation)location {
    [self.delegate customEventInterstitialDidDismiss:self];
}

- (void)didClickInterstitial:(CBLocation)location {
    [self.delegate customEventInterstitialWasClicked:self];
}

- (void)didDisplayInterstitial:(CBLocation)location {
    [self.delegate customEventInterstitialWillPresent:self];
}

@end
