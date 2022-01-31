//
//  UnityAdsMultiplexerDelegate.m
//  Pulp
//
//  Created by Mikhail Shulepov on 01/08/15.
//
//

#import "UnityAdsMultiplexerDelegate.h"
#import "WeakObject.h"

@interface UnityAdsMultiplexerDelegate()

@property (strong, nonatomic) NSMutableArray *delegates;

@end

@implementation UnityAdsMultiplexerDelegate

+ (instancetype)sharedInstance {
    static UnityAdsMultiplexerDelegate *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[UnityAdsMultiplexerDelegate alloc] init];
    });
    return instance;
}

- (instancetype)init {
    if (self = [super init]) {
        self.delegates = [NSMutableArray array];
        [[UnityAds sharedInstance] setDelegate:self];
    }
    return self;
}

- (void)addDelegate:(id<UnityAdsDelegate>)delegate {
    WeakObject *weakDelegate = [WeakObject with:delegate];
    [self.delegates addObject:weakDelegate];
}

- (void)removeDelegate:(id<UnityAdsDelegate>)delegate {
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

- (void)enumerateDelegatesThatRespondToSelector:(SEL)selector enumerator:(void(^)(id<UnityAdsDelegate> delegate))enumerator {
    NSArray *delegates = [self.delegates copy];
    for (WeakObject *delegate in delegates) {
        if ([delegate.object respondsToSelector:selector]) {
            enumerator(delegate.object);
        }
    }
}


#pragma mark - Delegate

- (void)unityAdsVideoCompleted:(NSString *)rewardItemKey skipped:(BOOL)skipped {
    [self enumerateDelegatesThatRespondToSelector:@selector(unityAdsVideoCompleted:skipped:) enumerator:^(id<UnityAdsDelegate> delegate) {
        [delegate unityAdsVideoCompleted:rewardItemKey skipped:skipped];
    }];
}

- (void)unityAdsWillShow {
    [self enumerateDelegatesThatRespondToSelector:@selector(unityAdsWillShow) enumerator:^(id<UnityAdsDelegate> delegate) {
        [delegate unityAdsWillShow];
    }];
}

- (void)unityAdsDidShow {
    [self enumerateDelegatesThatRespondToSelector:@selector(unityAdsDidShow) enumerator:^(id<UnityAdsDelegate> delegate) {
        [delegate unityAdsDidShow];
    }];
}

- (void)unityAdsWillHide {
    [self enumerateDelegatesThatRespondToSelector:@selector(unityAdsWillHide) enumerator:^(id<UnityAdsDelegate> delegate) {
        [delegate unityAdsWillHide];
    }];
}

- (void)unityAdsDidHide {
    [self enumerateDelegatesThatRespondToSelector:@selector(unityAdsDidHide) enumerator:^(id<UnityAdsDelegate> delegate) {
        [delegate unityAdsDidHide];
    }];
}

- (void)unityAdsWillLeaveApplication {
    [self enumerateDelegatesThatRespondToSelector:@selector(unityAdsWillLeaveApplication) enumerator:^(id<UnityAdsDelegate> delegate) {
        [delegate unityAdsWillLeaveApplication];
    }];
}

- (void)unityAdsVideoStarted {
    [self enumerateDelegatesThatRespondToSelector:@selector(unityAdsVideoStarted) enumerator:^(id<UnityAdsDelegate> delegate) {
        [delegate unityAdsVideoStarted];
    }];
}

- (void)unityAdsFetchCompleted {
    [self enumerateDelegatesThatRespondToSelector:@selector(unityAdsFetchCompleted) enumerator:^(id<UnityAdsDelegate> delegate) {
        [delegate unityAdsFetchCompleted];
    }];
}

- (void)unityAdsFetchFailed {
    [self enumerateDelegatesThatRespondToSelector:@selector(unityAdsFetchFailed) enumerator:^(id<UnityAdsDelegate> delegate) {
        [delegate unityAdsFetchFailed];
    }];
}

@end
