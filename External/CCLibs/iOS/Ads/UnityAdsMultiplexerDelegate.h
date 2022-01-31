//
//  UnityAdsMultiplexerDelegate.h
//  Pulp
//
//  Created by Mikhail Shulepov on 01/08/15.
//
//

#import <Foundation/Foundation.h>
#import <UnityAds/UnityAds.h>

@interface UnityAdsMultiplexerDelegate : NSObject<UnityAdsDelegate>


+ (instancetype)sharedInstance;

//delegates added weakly
- (void)addDelegate:(id<UnityAdsDelegate>)delegate;
- (void)removeDelegate:(id<UnityAdsDelegate>)delegate;

@end
