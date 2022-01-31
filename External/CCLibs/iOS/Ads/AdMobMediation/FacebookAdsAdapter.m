//
//  FacebookAdsAdapter.m
//  BlackHedgehog
//
//  Created by Mikhail Shulepov on 05/08/15.
//
//

#import "FacebookAdsAdapter.h"

@interface FacebookBannerAdsAdapter()

@property (strong, nonatomic) FBAdView *adView;

@end

@implementation FacebookBannerAdsAdapter

@synthesize delegate;

- (void)requestBannerAd:(GADAdSize)adSize
              parameter:(NSString *)serverParameter
                  label:(NSString *)serverLabel
                request:(GADCustomEventRequest *)request {
       
    FBAdSize fbAdSize = { .size = CGSizeZero };
    if (adSize.flags == kGADAdSizeSmartBannerPortrait.flags && CGSizeEqualToSize(adSize.size, kGADAdSizeSmartBannerPortrait.size)) {
        if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPhone) {
            fbAdSize = kFBAdSize320x50;
        } else {
            fbAdSize = kFBAdSizeHeight90Banner;
        }
    } else if (adSize.flags == kGADAdSizeSmartBannerLandscape.flags && CGSizeEqualToSize(adSize.size, kGADAdSizeSmartBannerLandscape.size)) {
        if ([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad) {
            fbAdSize = kFBAdSizeHeight90Banner;
        }
    } else {
        NSLog(@"Only smart banners supported");
    }
    
    if (CGSizeEqualToSize(fbAdSize.size, CGSizeZero)) {
        NSDictionary *errorInfo = @{NSLocalizedDescriptionKey: @"Can't find suitable size for FB banner"};
        NSError *error = [NSError errorWithDomain:@"FacebookBannerAdsAdapter" code:2001 userInfo:errorInfo];
        [self.delegate customEventBanner:self didFailAd:error];
        return;
    }
    
    UIViewController *vc = [[[UIApplication sharedApplication] delegate] window].rootViewController;
    self.adView = [[FBAdView alloc] initWithPlacementID:serverParameter
                                   adSize:fbAdSize
                       rootViewController:vc];
    self.adView.delegate = self;
    [self.adView loadAd];
    
    [self.adView setFrame:CGRectMake(0, 0, vc.view.frame.size.width, fbAdSize.size.height)];
}

#pragma mark -
#pragma mark Facebook Delegate

- (void)adViewDidClick:(FBAdView *)adView {
    [self.delegate customEventBannerWasClicked:self];
}

- (void)adViewDidFinishHandlingClick:(FBAdView *)adView {
    
}

- (void)adViewDidLoad:(FBAdView *)adView {
    [self.delegate customEventBanner:self didReceiveAd:self.adView];
}

- (void)adView:(FBAdView *)adView didFailWithError:(NSError *)error {
    [self.delegate customEventBanner:self didFailAd:error];
}

- (void)adViewWillLogImpression:(FBAdView *)adView {
    [self.delegate customEventBannerWillPresentModal:self];
}


@end

@interface FacebookInterstitialAdsAdapter()

@property (strong, nonatomic) FBInterstitialAd *interstitialAd;

@end

@implementation FacebookInterstitialAdsAdapter

@synthesize delegate;

- (void)requestInterstitialAdWithParameter:(NSString *)serverParameter
                                     label:(NSString *)serverLabel
                                   request:(GADCustomEventRequest *)request {
    self.interstitialAd = [[FBInterstitialAd alloc] initWithPlacementID:serverParameter];
    self.interstitialAd.delegate = self;
    [self.interstitialAd loadAd];
}

- (void)presentFromRootViewController:(UIViewController *)rootViewController {
    [self.interstitialAd showAdFromRootViewController:rootViewController];
}

- (void)interstitialAdDidClick:(FBInterstitialAd *)interstitialAd {
    [self.delegate customEventInterstitialWasClicked:self];
}

- (void)interstitialAdDidClose:(FBInterstitialAd *)interstitialAd {
    [self.delegate customEventInterstitialDidDismiss:self];
}

- (void)interstitialAdWillClose:(FBInterstitialAd *)interstitialAd {
    [self.delegate customEventInterstitialWillDismiss:self];
}

- (void)interstitialAdDidLoad:(FBInterstitialAd *)interstitialAd {
    [self.delegate customEventInterstitialDidReceiveAd:self];
}

- (void)interstitialAd:(FBInterstitialAd *)interstitialAd didFailWithError:(NSError *)error {
    [self.delegate customEventInterstitial:self didFailAd:error];
}

- (void)interstitialAdWillLogImpression:(FBInterstitialAd *)interstitialAd {

}

@end
