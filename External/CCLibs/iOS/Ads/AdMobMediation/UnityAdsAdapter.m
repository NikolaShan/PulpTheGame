//
//  UnityAdsAdapter.m
//  Pulp
//
//  Created by Mikhail Shulepov on 01/08/15.
//
//

#import "UnityAdsAdapter.h"
#import "UnityAdsMultiplexerDelegate.h"

@interface UnityAdsAdapter()

@property (strong, nonatomic) NSString *zoneId;

@end

@implementation UnityAdsAdapter

@synthesize delegate;

- (instancetype)init {
    if (self = [super init]) {
        [[UnityAdsMultiplexerDelegate sharedInstance] addDelegate:self];
        self.zoneId = @"defaultZone";
    }
    return self;
}

- (void)dealloc {
    [[UnityAdsMultiplexerDelegate sharedInstance] removeDelegate:self];
}


#pragma mark -
#pragma mark GADCustomEventInterstitial

- (void)requestInterstitialAdWithParameter:(NSString *)serverParameter
                                     label:(NSString *)serverLabel
                                   request:(GADCustomEventRequest *)request {
    
    if ([serverParameter length] > 0) {
        NSInteger percentage = [serverParameter integerValue];
        if (percentage > 0) {
            if (rand() % 100 > percentage) {
                NSDictionary *errorInfo = @{NSLocalizedDescriptionKey: @"Due to percentage restriction"};
                NSError *error = [NSError errorWithDomain:@"AdColonyAdapter" code:1001 userInfo:errorInfo];
                [self.delegate customEventInterstitial:self didFailAd:error];
                return;
            }
        }
    }

    if ([[UnityAds sharedInstance] canShowZone:self.zoneId]) {
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [self.delegate customEventInterstitialDidReceiveAd:self];
        });
    } else {
        NSDictionary *errorInfo = @{NSLocalizedDescriptionKey: @"UnityAds doesn't have ready ads for specified zone"};
        NSError *error = [NSError errorWithDomain:@"UnityAdsAdapter" code:1002 userInfo:errorInfo];
        [self.delegate customEventInterstitial:self didFailAd:error];
    }
}

- (void)presentFromRootViewController:(UIViewController *)rootViewController {
    [[UnityAds sharedInstance] setZone: self.zoneId];
    [[UnityAds sharedInstance] show];
}

#pragma mark - Delegate

- (void)unityAdsVideoCompleted:(NSString *)rewardItemKey skipped:(BOOL)skipped {}

- (void)unityAdsWillShow {
    [self.delegate customEventInterstitialWillPresent:self];
}

- (void)unityAdsWillHide {
    [self.delegate customEventInterstitialWillDismiss:self];
}

- (void)unityAdsDidHide {
    [self.delegate customEventInterstitialDidDismiss:self];
}

- (void)unityAdsWillLeaveApplication {
    [self.delegate customEventInterstitialWillLeaveApplication:self];
}

@end
