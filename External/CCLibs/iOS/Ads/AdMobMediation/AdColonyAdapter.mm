//
//  AdColonyAdapter.m
//  Square
//
//  Created by Mikhail Shulepov on 12/07/15.
//
//

#import "AdColonyAdapter.h"
#import "AdColonyMultiplexerDelegate.h"
#import "ObjcBridgingSupport.h"

#include "AdColonyX.h"

@interface AdColonyAdapter()

@property (strong, nonatomic) NSString *adColonyZoneId;

@end

@implementation AdColonyAdapter

@synthesize delegate;

- (instancetype)init {
    if (self = [super init]) {
        [[AdColonyMultiplexerDelegate sharedInstance] addDelegate:self];
    }
    return self;
}

- (void)dealloc {
    [[AdColonyMultiplexerDelegate sharedInstance] removeDelegate:self];
}


#pragma mark -
#pragma mark GADCustomEventInterstitial

- (void)requestInterstitialAdWithParameter:(NSString *)serverParameter
                                     label:(NSString *)serverLabel
                                   request:(GADCustomEventRequest *)request {

    auto zoneId = Advertise::AdColonyX::getInstance()->getDefaultInterstitialZoneId();
    if (zoneId.empty()) {
        NSDictionary *errorInfo = @{NSLocalizedDescriptionKey: @"Must be specified zoneId using AdColonyX::getInstance()->setDefaultInterstitialZoneId"};
        NSError *error = [NSError errorWithDomain:@"AdColonyAdapter" code:1000 userInfo:errorInfo];
        [self.delegate customEventInterstitial:self didFailAd:error];
        return;
    }
    
    if ([serverParameter length] > 0) {
        NSInteger percentage = [serverParameter integerValue];
        if (percentage > 0) {
            if (rand() % 100 >= percentage) {
                NSDictionary *errorInfo = @{NSLocalizedDescriptionKey: @"Due to percentage restriction"};
                NSError *error = [NSError errorWithDomain:@"AdColonyAdapter" code:1001 userInfo:errorInfo];
                [self.delegate customEventInterstitial:self didFailAd:error];
                return;
            }
        }
    }
    
    self.adColonyZoneId = NSStringFromStdString(zoneId);
    if ([AdColony zoneStatusForZone:self.adColonyZoneId] == ADCOLONY_ZONE_STATUS_ACTIVE) {
        [self.delegate customEventInterstitialDidReceiveAd:self];
    } else {
        NSDictionary *errorInfo = @{NSLocalizedDescriptionKey: @"AdColony doesn't have ready ads for specified zone"};
        NSError *error = [NSError errorWithDomain:@"AdColonyAdapter" code:1002 userInfo:errorInfo];
        [self.delegate customEventInterstitial:self didFailAd:error];
    }
}

- (void)presentFromRootViewController:(UIViewController *)rootViewController {
    [AdColony playVideoAdForZone:self.adColonyZoneId withDelegate:self];
}

- (void)onAdColonyAdStartedInZone:(NSString *)zoneID {
    [self.delegate customEventInterstitialWillPresent:self];
}

- (void)onAdColonyAdAttemptFinished:(BOOL)shown inZone:(NSString *)zoneID {
    [self.delegate customEventInterstitialDidDismiss:self];
}

@end
