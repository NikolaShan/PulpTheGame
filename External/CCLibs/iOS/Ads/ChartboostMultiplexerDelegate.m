//
//  ChartboostMultiplexerDelegate.m
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import "ChartboostMultiplexerDelegate.h"

#import "WeakObject.h"

@interface ChartboostMultiplexerDelegate()

@property (strong, nonatomic) NSMutableArray *delegates;

@end

@implementation ChartboostMultiplexerDelegate

+ (instancetype)sharedInstance {
    static ChartboostMultiplexerDelegate *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[ChartboostMultiplexerDelegate alloc] init];
    });
    return instance;
}

- (instancetype)init {
    if (self = [super init]) {
        self.delegates = [NSMutableArray array];
    }
    return self;
}

- (void)addDelegate:(id<ChartboostDelegate>)delegate {
    WeakObject *weakDelegate = [WeakObject with:delegate];
    [self.delegates addObject:weakDelegate];
}

- (void)removeDelegate:(id<ChartboostDelegate>)delegate {
    NSMutableIndexSet *toRemove = [NSMutableIndexSet indexSet];
    for (NSUInteger i = 0; i < self.delegates.count; ++i) {
        WeakObject *weakDelegate = self.delegates[i];
        if (weakDelegate.object == nil || weakDelegate.object == delegate) {
            [toRemove addIndex:i];
        }
    }
    [self.delegates removeObjectsAtIndexes:toRemove];
}


#pragma mark - Support


- (void)enumerateDelegatesThatRespondToSelector:(SEL)selector enumerator:(void(^)(id<ChartboostDelegate> delegate))enumerator {
    NSArray *delegates = [self.delegates copy];
    for (WeakObject *delegate in delegates) {
        if ([delegate.object respondsToSelector:selector]) {
            enumerator(delegate.object);
        }
    }
}

#pragma mark - Chartboost Delegate

- (BOOL)shouldRequestInterstitial:(CBLocation)location {
    return YES;
}

- (BOOL)shouldDisplayInterstitial:(CBLocation)location {
    return YES;
}

- (void)didDisplayInterstitial:(CBLocation)location {
    [self enumerateDelegatesThatRespondToSelector:@selector(didDisplayInterstitial:) enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didDisplayInterstitial:location];
    }];
}

- (void)didCacheInterstitial:(CBLocation)location {
    [self enumerateDelegatesThatRespondToSelector:@selector(didCacheInterstitial:) enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didCacheInterstitial:location];
    }];
}

- (void)didFailToLoadInterstitial:(CBLocation)location
                        withError:(CBLoadError)error {
    SEL selector = @selector(didFailToLoadInterstitial:withError:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didFailToLoadInterstitial:location withError:error];
    }];
}

- (void)didFailToRecordClick:(CBLocation)location
                   withError:(CBClickError)error {
    SEL selector = @selector(didFailToRecordClick:withError:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didFailToRecordClick:location withError:error];
    }];
}

- (void)didDismissInterstitial:(CBLocation)location {
    SEL selector = @selector(didDismissInterstitial:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didDismissInterstitial:location];
    }];
}

- (void)didCloseInterstitial:(CBLocation)location {
    SEL selector = @selector(didCloseInterstitial:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didCloseInterstitial:location];
    }];
}

- (void)didClickInterstitial:(CBLocation)location {
    SEL selector = @selector(didClickInterstitial:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didClickInterstitial:location];
    }];
}

- (BOOL)shouldDisplayMoreApps:(CBLocation)location {
    return YES;
}

- (void)didDisplayMoreApps:(CBLocation)location {
    SEL selector = @selector(didDisplayMoreApps:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didDisplayMoreApps:location];
    }];
}

- (void)didCacheMoreApps:(CBLocation)location {
    SEL selector = @selector(didCacheMoreApps:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didCacheMoreApps:location];
    }];
}

- (void)didDismissMoreApps:(CBLocation)location {
    SEL selector = @selector(didDismissMoreApps:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didDismissMoreApps:location];
    }];
}

- (void)didCloseMoreApps:(CBLocation)location {
    SEL selector = @selector(didCloseMoreApps:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didCloseMoreApps:location];
    }];
}

- (void)didClickMoreApps:(CBLocation)location {
    SEL selector = @selector(didClickMoreApps:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didClickMoreApps:location];
    }];
}

- (void)didFailToLoadMoreApps:(CBLocation)location
                    withError:(CBLoadError)error {
    SEL selector = @selector(didFailToLoadMoreApps:withError:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didFailToLoadMoreApps:location withError:error];
    }];
}

- (void)didPrefetchVideos {
    SEL selector = @selector(didPrefetchVideos);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didPrefetchVideos];
    }];
}

- (BOOL)shouldDisplayRewardedVideo:(CBLocation)location {
    return YES;
}

- (void)didDisplayRewardedVideo:(CBLocation)location {
    SEL selector = @selector(didDisplayRewardedVideo:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didDisplayRewardedVideo:location];
    }];
}

- (void)didCacheRewardedVideo:(CBLocation)location {
    SEL selector = @selector(didCacheRewardedVideo:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didCacheRewardedVideo:location];
    }];
}

- (void)didFailToLoadRewardedVideo:(CBLocation)location
                         withError:(CBLoadError)error {
    SEL selector = @selector(didFailToLoadRewardedVideo:withError:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didFailToLoadRewardedVideo:location withError:error];
    }];
}

- (void)didDismissRewardedVideo:(CBLocation)location {
    SEL selector = @selector(didDismissRewardedVideo:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didDismissRewardedVideo:location];
    }];
}

- (void)didCloseRewardedVideo:(CBLocation)location {
    SEL selector = @selector(didCloseRewardedVideo:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didCloseRewardedVideo:location];
    }];
}

- (void)didClickRewardedVideo:(CBLocation)location {
    SEL selector = @selector(didClickRewardedVideo:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didClickRewardedVideo:location];
    }];
}

- (void)didCompleteRewardedVideo:(CBLocation)location
                      withReward:(int)reward {
    SEL selector = @selector(didCompleteRewardedVideo:withReward:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didCompleteRewardedVideo:location withReward:reward];
    }];
}

- (void)didCacheInPlay:(CBLocation)location {
    SEL selector = @selector(didCacheInPlay:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didCacheInPlay:location];
    }];
}

- (void)didFailToLoadInPlay:(CBLocation)location
                  withError:(CBLoadError)error {
    SEL selector = @selector(didFailToLoadInPlay:withError:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didFailToLoadInPlay:location withError:error];
    }];
}

- (void)willDisplayVideo:(CBLocation)location {
    SEL selector = @selector(willDisplayVideo:);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate willDisplayVideo:location];
    }];
}

- (void)didCompleteAppStoreSheetFlow {
    SEL selector = @selector(didCompleteAppStoreSheetFlow);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didCompleteAppStoreSheetFlow];
    }];
}

- (void)didPauseClickForConfirmation {
    SEL selector = @selector(didPauseClickForConfirmation);
    [self enumerateDelegatesThatRespondToSelector:selector enumerator:^(id<ChartboostDelegate> delegate) {
        [delegate didPauseClickForConfirmation];
    }];
}

@end
