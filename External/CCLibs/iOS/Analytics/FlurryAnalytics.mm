//
//  FlurryAnalytics.cpp
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Flurry.h"

#include "FlurryAnalytics.h"
#include "ObjcBridgingSupport.h"

class IOSFlurryAnalytics: public FlurryAnalytics {
public:
    virtual void startSession(const std::string &apiKey) override {
        [Flurry startSession:NSStringFromStdString(apiKey)];
    }
    
    virtual void logPageView(const std::string &pageName) override {
        logEvent("Screen view", {{"screen", pageName}});
        [Flurry logPageView];
    }
    
    virtual void logEvent(const std::string &event) override {
        [Flurry logEvent:NSStringFromStdString(event)];
    }
    
    virtual void logEvent(const std::string &event, const std::map<std::string, std::string> &parameters) override {
        [Flurry logEvent:NSStringFromStdString(event)
          withParameters:NSDictionaryFromStdMap(parameters)];
    }
    
    virtual void logTimedEvent(const std::string &event) override {
        [Flurry logEvent:NSStringFromStdString(event) timed:YES];
    }
    
    virtual void logTimedEvent(const std::string &event, const std::map<std::string, std::string> &parameters) override {
        [Flurry logEvent:NSStringFromStdString(event)
          withParameters:NSDictionaryFromStdMap(parameters)
                   timed:YES];
    }
    
    virtual void endTimedEvent(const std::string &event) override {
        [Flurry endTimedEvent:NSStringFromStdString(event) withParameters:[NSDictionary dictionary]];
    }
    
    virtual void endTimedEvent(const std::string &event, const std::map<std::string, std::string> &parameters) override {
        [Flurry endTimedEvent:NSStringFromStdString(event)
               withParameters:NSDictionaryFromStdMap(parameters)];
    }
};

std::shared_ptr<FlurryAnalytics> FlurryAnalytics::getInstance() {
    static auto instance = std::dynamic_pointer_cast<FlurryAnalytics>(std::make_shared<IOSFlurryAnalytics>());
    return instance;
}