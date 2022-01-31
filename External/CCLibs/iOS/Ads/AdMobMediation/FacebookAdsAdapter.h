//
//  FacebookAdsAdapter.h
//  BlackHedgehog
//
//  Created by Mikhail Shulepov on 05/08/15.
//
//

#import <Foundation/Foundation.h>

#import <GoogleMobileAds/GADCustomEventBanner.h>
#import <GoogleMobileAds/GADCustomEventInterstitial.h>
#import <FBAudienceNetwork/FBAdView.h>
#import <FBAudienceNetwork/FBInterstitialAd.h>

@interface FacebookBannerAdsAdapter : NSObject<GADCustomEventBanner, FBAdViewDelegate>

@end

@interface FacebookInterstitialAdsAdapter : NSObject<GADCustomEventInterstitial, FBInterstitialAdDelegate>

@end
