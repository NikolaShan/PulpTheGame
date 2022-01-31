//
//  TuneAnalytics.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 10/09/15.
//
//

#import <Tune/Tune.h>
#import <AdSupport/AdSupport.h>

#import "AppDelegateMultiplexer.h"
#import "ObjcBridgingSupport.h"

#include "TuneAnalytics.h"


@interface IOSTuneAnalyticsAppDelegate: NSObject<UIApplicationDelegate>

@property (strong, nonatomic) NSString *advertiserId;
@property (strong, nonatomic) NSString *conversionKey;
@property (nonatomic) BOOL existingUser;

@end

@implementation IOSTuneAnalyticsAppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    [Tune initializeWithTuneAdvertiserId:self.advertiserId tuneConversionKey:self.conversionKey];
    [Tune automateIapEventMeasurement:YES];
    if (self.existingUser) {
        [Tune setExistingUser:YES];
    }
    return YES;
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [Tune measureSession];
}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    [Tune applicationDidOpenURL:[url absoluteString] sourceApplication:sourceApplication];
    return NO;
}

@end

class IOSTuneAnalytics: public TuneAnalytics {
public:
    virtual void start(const std::string &advertiserId, const std::string &conversionKey, bool existingUser) override {
        IOSTuneAnalyticsAppDelegate *tuneAppDelegate = [[IOSTuneAnalyticsAppDelegate alloc] init];
        tuneAppDelegate.advertiserId = NSStringFromStdString(advertiserId);
        tuneAppDelegate.conversionKey = NSStringFromStdString(conversionKey);
        tuneAppDelegate.existingUser = existingUser;
        [[AppDelegateMultiplexer shared] addApplicationDelegateToBeginning:tuneAppDelegate];
    }
    
    void setupUserInformation() {
        
    }
    
    virtual void measureLevelAchieved(int levelNumber) override {
        setupUserInformation();
        TuneEvent *event = [TuneEvent eventWithName:TUNE_EVENT_LEVEL_ACHIEVED];
        event.level = levelNumber;
        
        [Tune measureEvent:event];
    }
};

std::shared_ptr<TuneAnalytics> TuneAnalytics::getInstance() {
    static auto instance = std::make_shared<IOSTuneAnalytics>();
    return instance;
}
