//
//  IOSAdColonyDelegate.h
//  Square
//
//  Created by Mikhail Shulepov on 12/07/15.
//
//

#import <Foundation/Foundation.h>
#import <AdColony/AdColony.h>

@interface AdColonyMultiplexerDelegate : NSObject<AdColonyDelegate>

+ (instancetype)sharedInstance;

- (void)addDelegate:(id<AdColonyDelegate>)delegate;
- (void)removeDelegate:(id<AdColonyDelegate>)delegate;

@end
