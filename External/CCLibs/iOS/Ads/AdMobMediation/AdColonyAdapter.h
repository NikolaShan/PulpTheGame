//
//  AdColonyAdapter.h
//  Square
//
//  Created by Mikhail Shulepov on 12/07/15.
//
//

#import <Foundation/Foundation.h>
#import <GoogleMobileAds/GADCustomEventInterstitial.h>
#import <AdColony/AdColony.h>

@interface AdColonyAdapter : NSObject<GADCustomEventInterstitial, AdColonyDelegate, AdColonyAdDelegate>

@end
