//
//  UnityAdsAdapter.h
//  Pulp
//
//  Created by Mikhail Shulepov on 01/08/15.
//
//

#import <Foundation/Foundation.h>
#import <GoogleMobileAds/GADCustomEventInterstitial.h>
#import <UnityAds/UnityAds.h>

@interface UnityAdsAdapter : NSObject<GADCustomEventInterstitial, UnityAdsDelegate>

@end
