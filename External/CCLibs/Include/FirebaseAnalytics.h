//
//  FirebaseAnalytics.h
//  ColorTilt
//
//  Created by Mikhail Shulepov on 21/09/16.
//
//

#pragma once

#include <string>

#include "Analytics.h"

class FirebaseAnalytics: public std::enable_shared_from_this<FirebaseAnalytics> {
    public:
    /// App Open event. By logging this event when an App is moved to the foreground, developers can
    /// understand how often users leave and return during the course of a Session. Although Sessions
    /// are automatically reported, this event can provide further clarification around the continuous
    /// engagement of app-users.
    static const std::string EventAppOpen;
    
    
    static const std::string EventLevelUp;
    static const std::string EventPostScore;
    static const std::string EventShare;
    static const std::string EventUnlockAchievement;
    
    virtual ~FirebaseAnalytics() {}
    
    static std::shared_ptr<FirebaseAnalytics> getInstance();
    
    virtual void start() = 0;
    
    //sample: (ui_action, button_press, play, Value::Null)
    virtual void sendEvent(const std::string &name,
                           const std::map<std::string, std::string> &parameters) = 0;
    
    protected:
    FirebaseAnalytics() {}
    
    public:
    class Provider: public AnalyticsProvider {
        public:
        Provider(std::shared_ptr<FirebaseAnalytics> fa): firebaseAnalytics(fa) {
            
        }
        
        virtual void sendEvent(const std::string &name,
                               const std::map<std::string, std::string> &parameters) override {
            firebaseAnalytics->sendEvent(name, parameters);
        }
        
        virtual void trackScreenView(const std::string &name) override {
            //TODO:
        }
        
        private:
        std::shared_ptr<FirebaseAnalytics> firebaseAnalytics;
    };
    
    std::shared_ptr<Provider> makeProvider() {
        return std::make_shared<Provider>(shared_from_this());
    }
};
