//
//  LocalNotificationManager.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 11/06/15.
//
//

#import <UIKit/UIKit.h>

#import "AppDelegateMultiplexer.h"
#import "ObjcBridgingSupport.h"

#include "NotificationsManager.h"

#pragma mark - Notifications App Delegate

@interface NotificationsAppDelegate : NSObject<UIApplicationDelegate>

@property (nonatomic, assign) std::shared_ptr<NotificationsManager::Handler> handler;
@property (nonatomic, assign) BOOL shouldRegisterForNotifications;

@end

@implementation NotificationsAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    if (self.shouldRegisterForNotifications) {
        if ([application respondsToSelector:@selector(registerUserNotificationSettings:)]) {
            UIUserNotificationType types = UIUserNotificationTypeAlert | UIUserNotificationTypeBadge | UIUserNotificationTypeSound;
            UIUserNotificationSettings *settings = [UIUserNotificationSettings settingsForTypes:types categories:nil];
            [application registerUserNotificationSettings:settings];
            
        } else {
            //TODO: remote notifications?
        }
        
        UILocalNotification *localNotif = launchOptions[UIApplicationLaunchOptionsLocalNotificationKey];
        if (localNotif) {
            [self handleNotificationWithInfo:localNotif.userInfo];
        }
        
        NSDictionary *remoteNotif = launchOptions[UIApplicationLaunchOptionsRemoteNotificationKey];
        if (remoteNotif) {
            [self handleNotificationWithInfo:remoteNotif];
        }
    }
    return YES;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [application setApplicationIconBadgeNumber:0];
}

- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings {
    //nothing to do
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {
    [self handleNotificationWithInfo:userInfo];
}

- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification {
    [self handleNotificationWithInfo:notification.userInfo];
}

- (void)handleNotificationWithInfo:(NSDictionary *)info {
    if (!self.handler) {
        NSAssert(NO, @"Error: received notification, but handler wasn't setup!");
    } else {
        const cocos2d::ValueMap valueMap = ValueMapFromNSDictionary(info);
        const int tag = [info[@"tag"] intValue];
        self.handler->hanleNotification(tag, valueMap);
    }
}

@end


#pragma mark - Implementation

class IOSNotificationsManager: public NotificationsManager {
private:
    NotificationsAppDelegate *appDelegate;
    
private:
    void registerForNotifications() {
        appDelegate.shouldRegisterForNotifications = YES;
    }
    
    bool canSchedule() const {
        if ([UIApplication instancesRespondToSelector:@selector(currentUserNotificationSettings)]) {
            UIUserNotificationSettings *settings = [UIApplication sharedApplication].currentUserNotificationSettings;
            return settings.types != UIUserNotificationTypeNone;
        }
        return true;
    }

    
public:
    IOSNotificationsManager(std::shared_ptr<Handler> handler, bool registerFirstSession) {
        appDelegate = [[NotificationsAppDelegate alloc] init];
        appDelegate.handler = handler;
        [[AppDelegateMultiplexer shared] addApplicationDelegate:appDelegate];
        
        bool shouldRegister = registerFirstSession;
        if (!registerFirstSession) {
            NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
            shouldRegister = [ud boolForKey:@"NMWasLaunchedEarlier"];
            [ud setBool:YES forKey:@"NMWasLaunchedEarlier"];
        }
        if (shouldRegister) {
            registerForNotifications();
        }
    }
    
    virtual void scheduleLocalNotification(int tag, const std::string &message, const std::string &action, int delayInSeconds, const cocos2d::ValueMap &info) override {
        if (!canSchedule()) {
            cocos2d::log("Can't schedule local notifications: not allowed by the user");
            return;
        }
        
        cancelLocalNotifications({tag});
        
        UILocalNotification *notification = [[UILocalNotification alloc] init];
        notification.fireDate = [NSDate dateWithTimeIntervalSinceNow:delayInSeconds];
        notification.alertBody = NSStringFromStdString(message);
        if (!action.empty()) {
            notification.alertAction = NSStringFromStdString(action);
        }
        NSMutableDictionary *infoDict = [NSDictionaryFromValueMap(info) mutableCopy];
        [infoDict setObject:@(tag) forKey:@"tag"];
        notification.userInfo = infoDict;
        [[UIApplication sharedApplication] scheduleLocalNotification:notification];
        
        NSLog(@"Did schedule local notification, fire date: %@", [notification.fireDate description]);
    }
    
    virtual void cancelLocalNotifications(const std::vector<int> &tags) override {
        NSArray<UILocalNotification *> *notifications = [[UIApplication sharedApplication] scheduledLocalNotifications];
        for (UILocalNotification *notification in notifications) {
            NSDictionary *userInfo = notification.userInfo;
            const int tag = [userInfo[@"tag"] intValue];
            auto it = std::find(tags.begin(), tags.end(), tag);
            if (it != tags.end()) {
                [[UIApplication sharedApplication] cancelLocalNotification:notification];
            }
        }
    }
};

NotificationsManager *NotificationsManager::instance = nullptr;

NotificationsManager *NotificationsManager::create(std::shared_ptr<Handler> handler, bool registerFirstSession) {
    return new IOSNotificationsManager(handler, registerFirstSession);
}