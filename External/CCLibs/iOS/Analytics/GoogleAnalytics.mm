//
//  GoogleAnalytics.cpp
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "GAI.h"
#import "GAIFields.h"
#import "GAIDictionaryBuilder.h"

#include "GoogleAnalytics.h"
#include "ObjcBridgingSupport.h"

class IOSGoogleAnalytics: public GoogleAnalytics {
private:
    id<GAITracker> tracker;
    
public:
    virtual void startWithTrackerId(const std::string &trackerId) override {
        id<GAITracker> tracker = [[GAI sharedInstance] trackerWithTrackingId:NSStringFromStdString(trackerId)];
        if ([GAI sharedInstance].defaultTracker == nil) {
            [[GAI sharedInstance] setDefaultTracker:tracker];
        }
        this->tracker = tracker;
    }
    
    virtual void enableDebugLogging(bool enable) override {
        [[[GAI sharedInstance] logger] setLogLevel:enable ? kGAILogLevelVerbose : kGAILogLevelNone];
    }
    
    virtual void enableIDFACollection(bool enable) override {
        [tracker setAllowIDFACollection:enable];
    }
    
    virtual void setDispatchInterval(float seconds) override {
        [[GAI sharedInstance] setDispatchInterval:seconds];
    }
    
    virtual void set(const std::string &key, const std::string &value) override {
        NSString *nsKey = NSStringFromStdString(key);
        NSString *nsValue = NSStringFromStdString(value);
        [tracker set:nsKey value:nsValue];
    }
    
    //sample: (ui_action, button_press, play, Value::Null)
    virtual void sendEvent(const std::string &category,
                           const std::string &action,
                           const std::string &label) override {
        NSString *nsCategory = NSStringFromStdString(category);
        NSString *nsAction = NSStringFromStdString(action);
        NSString *nsLabel = label.empty() ? nil : NSStringFromStdString(label);
        GAIDictionaryBuilder *builder =
        [GAIDictionaryBuilder createEventWithCategory:nsCategory
                                               action:nsAction
                                                label:nsLabel
                                                value:nil];
        [tracker send:[builder build]];
    }
    
    virtual void sendEvent(const std::string &category,
                           const std::string &action,
                           const std::string &label,
                           int value) override {
        NSString *nsCategory = NSStringFromStdString(category);
        NSString *nsAction = NSStringFromStdString(action);
        NSString *nsLabel = label.empty() ? nil : NSStringFromStdString(label);
        NSNumber *nsValue = @(value);
        GAIDictionaryBuilder *builder =
        [GAIDictionaryBuilder createEventWithCategory:nsCategory
                                               action:nsAction
                                                label:nsLabel
                                                value:nsValue];
        [tracker send:[builder build]];
    }
    
    //sample: (main_menu)
    virtual void setScreen(const std::string &name) override {
        NSString *nsName = NSStringFromStdString(name);
        [tracker set:kGAIScreenName value:nsName];
        NSDictionary *screenViewEvent = [[GAIDictionaryBuilder createScreenView] build];
        [tracker send: screenViewEvent];
    }
    
    //sample: (resources, loadTime, high_scores, "")
    virtual void sendTiming(const std::string &category,
                            long intervalMs,
                            const std::string &name,
                            const std::string &label = "") override {
        NSString *nsCategory = NSStringFromStdString(category);
        NSNumber *nsIntervalMs = @(intervalMs);
        NSString *nsName = NSStringFromStdString(name);
        NSString *nsLabel = label.empty() ? nil : NSStringFromStdString(label);
        GAIDictionaryBuilder *builder =
        [GAIDictionaryBuilder createTimingWithCategory:nsCategory
                                              interval:nsIntervalMs
                                                  name:nsName
                                                 label:nsLabel];
        NSDictionary *params = [builder build];
        [tracker send:params];
    }
};

std::shared_ptr<GoogleAnalytics> GoogleAnalytics::getInstance() {
    static auto instance = std::dynamic_pointer_cast<GoogleAnalytics>(std::make_shared<IOSGoogleAnalytics>());
    return instance;
}

