//
//  IOSAdColonyDelegate.m
//  Square
//
//  Created by Mikhail Shulepov on 12/07/15.
//
//

#import "AdColonyMultiplexerDelegate.h"
#import "WeakObject.h"

@interface AdColonyMultiplexerDelegate()

@property (strong, nonatomic) NSMutableArray *delegates;

@end

@implementation AdColonyMultiplexerDelegate

+ (instancetype)sharedInstance {
    static AdColonyMultiplexerDelegate *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[AdColonyMultiplexerDelegate alloc] init];
    });
    return instance;
}

- (instancetype)init {
    if (self = [super init]) {
        self.delegates = [NSMutableArray array];
    }
    return self;
}

- (void)addDelegate:(id<AdColonyDelegate>)delegate {
    WeakObject *weakDelegate = [WeakObject with:delegate];
    [self.delegates addObject:weakDelegate];
}

- (void)removeDelegate:(id<AdColonyDelegate>)delegate {
    NSMutableIndexSet *toRemove = [NSMutableIndexSet indexSet];
    for (NSUInteger i = 0; i < self.delegates.count; ++i) {
        WeakObject *weakDelegate = self.delegates[i];
        if (weakDelegate.object == nil || weakDelegate.object == delegate) {
            [toRemove addIndex:i];
        }
    }
    [self.delegates removeObjectsAtIndexes:toRemove];
}

- (void)enumerateDelegatesThatRespondToSelector:(SEL)selector enumerator:(void(^)(id<AdColonyDelegate> delegate))enumerator {
    NSArray *delegates = [self.delegates copy];
    for (WeakObject *delegate in delegates) {
        if ([delegate.object respondsToSelector:selector]) {
            enumerator(delegate.object);
        }
    }
}

- (void)onAdColonyAdAvailabilityChange:(BOOL)available inZone:(NSString *)zoneID {
    [self enumerateDelegatesThatRespondToSelector:@selector(onAdColonyAdAvailabilityChange:inZone:) enumerator:^(id<AdColonyDelegate> delegate) {
        [delegate onAdColonyAdAvailabilityChange:available inZone:zoneID];
    }];
}

- (void)onAdColonyV4VCReward:(BOOL)success currencyName:(NSString *)currencyName currencyAmount:(int)amount inZone:(NSString *)zoneID {
    [self enumerateDelegatesThatRespondToSelector:@selector(onAdColonyV4VCReward:currencyName:currencyAmount:inZone:) enumerator:^(id<AdColonyDelegate> delegate) {
        [delegate onAdColonyV4VCReward:success currencyName:currencyName currencyAmount:amount inZone:zoneID];
    }];
}

@end