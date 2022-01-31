//
//  ChartboostMultiplexerDelegate.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Chartboost/Chartboost.h>

@interface ChartboostMultiplexerDelegate : NSObject<ChartboostDelegate>

+ (instancetype)sharedInstance;

//delegates added weakly
- (void)addDelegate:(id<ChartboostDelegate>)delegate;
- (void)removeDelegate:(id<ChartboostDelegate>)delegate;

@end
