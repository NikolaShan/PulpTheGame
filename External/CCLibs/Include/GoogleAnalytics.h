//
//  GoogleAnalytics.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#ifndef __CocosFrameworksBinding__GoogleAnalytics__
#define __CocosFrameworksBinding__GoogleAnalytics__

#include <string>

#include "AnalyticsDeprecated.h"

class GoogleAnalytics: public std::enable_shared_from_this<GoogleAnalytics> {
public:
    virtual ~GoogleAnalytics() {}
    
    static std::shared_ptr<GoogleAnalytics> getInstance();
    
    virtual void startWithTrackerId(const std::string &trackerId) = 0;
    
    virtual void enableDebugLogging(bool enable) = 0;
    virtual void enableIDFACollection(bool enable) = 0;
    virtual void setDispatchInterval(float seconds) = 0;
    
    virtual void set(const std::string &key, const std::string &value) = 0;
    
    //sample: (ui_action, button_press, play, Value::Null)
    virtual void sendEvent(const std::string &category,
                           const std::string &action,
                           const std::string &label) = 0;
                   
    virtual void sendEvent(const std::string &category,
                           const std::string &action,
                           const std::string &label,
                           int value) = 0;
    
    //sample: (main_menu)
    virtual void setScreen(const std::string &name) = 0;
    
    //sample: (resources, loadTime, high_scores, "")
    virtual void sendTiming(const std::string &category,
                            long intervalMs,
                            const std::string &name,
                            const std::string &label = "") = 0;
    
protected:
    GoogleAnalytics() {}
    
public:
    class Provider: public AnalyticsProvider {
    public:
        Provider(std::shared_ptr<GoogleAnalytics> ga): googleAnalytics(ga) {
            
        }
        
        virtual void sendEvent(const std::string &category,
                               const std::string &action,
                               const std::string &label,
                               int value) override {
            googleAnalytics->sendEvent(category, action, label, value);
        }
        
        virtual void sendEvent(const std::string &category,
                               const std::string &action,
                               const std::string &label) override {
            googleAnalytics->sendEvent(category, action, label);
        }
        
        virtual void trackScreenView(const std::string &name) override {
            googleAnalytics->setScreen(name);
        }
        
    private:
        std::shared_ptr<GoogleAnalytics> googleAnalytics;
    };
    
    std::shared_ptr<Provider> makeProvider() {
        return std::make_shared<Provider>(shared_from_this());
    }
};

#endif /* defined(__CocosFrameworksBinding__GoogleAnalytics__) */
