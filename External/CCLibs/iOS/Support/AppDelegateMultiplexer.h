//
//  AppDelegateMultiplexer.h
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#import <UIKit/UIKit.h>
#import "AppController.h"

@interface AppDelegateMultiplexer : AppController

+ (AppDelegateMultiplexer *)shared;

// Delegates are added with a strong reference !!!
- (void)removeApplicationDelegate:(id<UIApplicationDelegate>)delegate;
- (void)addApplicationDelegate:(id<UIApplicationDelegate>)delegate;
- (void)addApplicationDelegateToBeginning:(id<UIApplicationDelegate>)delegate;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions;

- (void)applicationDidEnterBackground:(UIApplication *)application;
- (void)applicationWillEnterForeground:(UIApplication *)application;
- (void)applicationDidBecomeActive:(UIApplication *)application;
- (void)applicationWillResignActive:(UIApplication *)application;
- (void)applicationWillTerminate:(UIApplication *)application;

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation;

- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings;
- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken;
- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error;
- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo;
- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification;

@end
