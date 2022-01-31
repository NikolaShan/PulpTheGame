//
//  Analytics.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#ifndef CocosFrameworksBinding_Analytics_h
#define CocosFrameworksBinding_Analytics_h

#include <string>
#include <vector>
#include <memory>

class AnalyticsProvider {
public:
    virtual void sendEvent(const std::string &category,
                   const std::string &action,
                   const std::string &label) = 0;

    virtual void sendEvent(const std::string &category,
                           const std::string &action,
                           const std::string &label,
                           int value) = 0;
    
    virtual void trackScreenView(const std::string &name) = 0;
};

class Analytics {
public:
    static std::shared_ptr<Analytics> getInstance() {
        static std::shared_ptr<Analytics> instance = std::make_shared<Analytics>();
        return instance;
    }
    
    void addProvider(std::shared_ptr<AnalyticsProvider> provider) {
        providers.push_back(provider);
    }
    
    void sendEvent(const std::string &category,
                              const std::string &action,
                              const std::string &label,
                              const int value) {
        for (auto provider: providers) {
            if (provider.get()) {
                provider->sendEvent(category, action, label, value);
            }
        }
    }
    
    void sendEvent(const std::string &category,
                              const std::string &action,
                              const std::string &label) {
        for (auto provider: providers) {
            if (provider.get()) {
                provider->sendEvent(category, action, label);
            }
        }
    }
    
    void trackScreenView(const std::string &name) {
        for (auto provider: providers) {
            if (provider.get()) {
                provider->trackScreenView(name);
            }
        }
    }
    
    
    void startSession();
    //returns true if started new session
    bool startOrResumeSession();
    void endSession();
    
private:
    void submitSessionLength();
    
private:
    std::vector<std::shared_ptr<AnalyticsProvider>> providers;
    
    static const std::string START_TIME_KEY;
    static const std::string END_TIME_KEY;
};




#endif
