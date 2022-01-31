//
//  AppDelegateMultiplexer.m
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#import <AVFoundation/AVFoundation.h>
#import "AppDelegateMultiplexer.h"

@interface AppDelegateMultiplexer()

@property (strong, nonatomic) NSMutableArray *delegates;

@end

@implementation AppDelegateMultiplexer

+ (AppDelegateMultiplexer *)shared {
    id<UIApplicationDelegate> delegate = [UIApplication sharedApplication].delegate;
    if ([delegate isKindOfClass:[AppDelegateMultiplexer class]]) {
        return delegate;
    }
    NSAssert(NO, @"App must be started with AppDelegateMultiplexer as delegate. Visit main.m for fix");
    return nil;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        self.delegates = [NSMutableArray array];
    }
    return self;
}

- (void)removeApplicationDelegate:(id<UIApplicationDelegate>)delegate {
    [self.delegates removeObject:delegate];
}

- (void)addApplicationDelegate:(id<UIApplicationDelegate>)delegate {
    [self.delegates addObject:delegate];
}

- (void)addApplicationDelegateToBeginning:(id<UIApplicationDelegate>)delegate {
    if ([self.delegates count] > 0) {
        [self.delegates insertObject:delegate atIndex:0];
    } else {
        [self.delegates addObject:delegate];
    }
}

#pragma mark Support

- (void)enumerateDelegatesThatRespondToSelector:(SEL)selector enumerator:(void(^)(id<UIApplicationDelegate> delegate))enumerator {
    NSArray *delegates = self.delegates;
    for (id<UIApplicationDelegate> delegate in delegates) {
        if ([delegate respondsToSelector:selector]) {
            enumerator(delegate);
        }
    }
}

#pragma mark Lifecycle

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:nil];
    
    [super application:application didFinishLaunchingWithOptions:launchOptions];
    
    [self enumerateDelegatesThatRespondToSelector:@selector(application:didFinishLaunchingWithOptions:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate application:application didFinishLaunchingWithOptions:launchOptions];
    }];
    
    return YES;
}

- (void)applicationDidEnterBackground:(UIApplication *)application  {
    if ([super respondsToSelector:@selector(applicationDidEnterBackground:)]) {
        [super applicationDidEnterBackground:application];
    }
    [self enumerateDelegatesThatRespondToSelector:@selector(applicationDidEnterBackground:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate applicationDidEnterBackground:application];
    }];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:nil];
    if ([super respondsToSelector:@selector(applicationWillEnterForeground:)]) {
        [super applicationWillEnterForeground:application];
    }
    [self enumerateDelegatesThatRespondToSelector:@selector(applicationWillEnterForeground:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate applicationWillEnterForeground:application];
    }];
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:nil];
    if ([super respondsToSelector:@selector(applicationDidBecomeActive:)]) {
        [super applicationDidBecomeActive:application];
    }
    [self enumerateDelegatesThatRespondToSelector:@selector(applicationDidBecomeActive:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate applicationDidBecomeActive:application];
    }];
}

- (void)applicationWillResignActive:(UIApplication *)application {
    if ([super respondsToSelector:@selector(applicationWillResignActive:)]) {
        [super applicationWillResignActive:application];
    }
    [self enumerateDelegatesThatRespondToSelector:@selector(applicationWillResignActive:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate applicationWillResignActive:application];
    }];
}

- (void)applicationWillTerminate:(UIApplication *)application {
    if ([super respondsToSelector:@selector(applicationWillTerminate:)]) {
        [super applicationWillTerminate:application];
    }
    [self enumerateDelegatesThatRespondToSelector:@selector(applicationWillTerminate:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate applicationWillTerminate:application];
    }];
}

#pragma mark Urls

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    if ([super respondsToSelector:@selector(application:openURL:sourceApplication:annotation:)]) {
        if ([super application:application openURL:url sourceApplication:sourceApplication annotation:annotation]) {
            return YES;
        }
    }
    
    NSArray *delegates = self.delegates;
    for (id<UIApplicationDelegate> delegate in delegates) {
        if ([delegate respondsToSelector:@selector(application:openURL:sourceApplication:annotation:)]) {
            if ([delegate application:application openURL:url sourceApplication:sourceApplication annotation:annotation]) {
                return YES;
            }
        }
    }
    return NO;
}

- (BOOL)application:(UIApplication *)app openURL:(NSURL *)url options:(NSDictionary<NSString *,id> *)options {
    NSArray *delegates = self.delegates;
    for (id<UIApplicationDelegate> delegate in delegates) {
        if ([delegate respondsToSelector:@selector(application:openURL:options:)]) {
            if ([delegate application:app openURL:url options:options]) {
                return YES;
            }
        }
    }
    return NO;
}


#pragma mark Notifications

- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings  {
    [self enumerateDelegatesThatRespondToSelector:@selector(application:didRegisterUserNotificationSettings:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate application:application didRegisterUserNotificationSettings:notificationSettings];
    }];
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    [self enumerateDelegatesThatRespondToSelector:@selector(application:didRegisterForRemoteNotificationsWithDeviceToken:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate application:application didRegisterForRemoteNotificationsWithDeviceToken:deviceToken];
    }];
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
    [self enumerateDelegatesThatRespondToSelector:@selector(application:didFailToRegisterForRemoteNotificationsWithError:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate application:application didFailToRegisterForRemoteNotificationsWithError:error];
    }];
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {
    [self enumerateDelegatesThatRespondToSelector:@selector(application:didReceiveRemoteNotification:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate application:application didReceiveRemoteNotification:userInfo];
    }];
}

- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification {
    [self enumerateDelegatesThatRespondToSelector:@selector(application:didReceiveLocalNotification:) enumerator:^(id<UIApplicationDelegate> delegate) {
        [delegate application:application didReceiveLocalNotification:notification];
    }];
}

@end
