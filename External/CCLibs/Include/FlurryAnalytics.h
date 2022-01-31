//
//  FlurryAnalytics.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#ifndef __CocosFrameworksBinding__FlurryAnalytics__
#define __CocosFrameworksBinding__FlurryAnalytics__

#include "Analytics.h"
#include <string>
#include <map>
#include "Utils/StringUtils.h"

class FlurryAnalytics {   
public:
    static std::shared_ptr<FlurryAnalytics> getInstance();
    
    virtual ~FlurryAnalytics() {}
    
    virtual void startSession(const std::string &apiKey) = 0;
    
    virtual void logPageView(const std::string &pageName) = 0;
    
    virtual void logEvent(const std::string &event) = 0;
    virtual void logEvent(const std::string &event, const std::map<std::string, std::string> &parameters) = 0;
    
    virtual void logTimedEvent(const std::string &event) = 0;
    virtual void logTimedEvent(const std::string &event, const std::map<std::string, std::string> &parameters) = 0;
    virtual void endTimedEvent(const std::string &event) = 0;
    virtual void endTimedEvent(const std::string &event, const std::map<std::string, std::string> &parameters) = 0;
    
protected:
    FlurryAnalytics() {}
       
public:
    class Provider: public AnalyticsProvider {
    public:
        virtual void sendEvent(const std::string &category,
                               const std::string &action,
                               const std::string &label,
                               int value) override {
            std::map<std::string, std::string> params = {
                {"category", category},
                {"label", label},
                {"value", StringUtils::toString(value)}
            };
            FlurryAnalytics::getInstance()->logEvent(action, params);
        }
        
        virtual void sendEvent(const std::string &category,
                               const std::string &action,
                               const std::string &label) override {
            std::map<std::string, std::string> params = { {"category", category}, {"label", label} };
            FlurryAnalytics::getInstance()->logEvent(action, params);
        }
        
        virtual void trackScreenView(const std::string &name) override {
            FlurryAnalytics::getInstance()->logPageView(name);
        }
    };
    
    std::shared_ptr<Provider> makeProvider() {
        return std::make_shared<Provider>();
    }
};

#endif /* defined(__CocosFrameworksBinding__FlurryAnalytics__) */
