//
//  ChartboostAdapter.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <GoogleMobileAds/GADCustomEventInterstitial.h>
#import <Chartboost/Chartboost.h>

@interface ChartboostAdapter : NSObject<GADCustomEventInterstitial, ChartboostDelegate>

@end
