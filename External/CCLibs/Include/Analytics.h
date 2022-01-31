//
//  Analytics.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

class AnalyticsProvider {
public:
    
    virtual void sendEvent(const std::string &name,
                           const std::map<std::string, std::string> &parameters) = 0;
    
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
    
    void sendEvent(const std::string &name,
                   const std::map<std::string, std::string> &parameters) {
        for (auto provider: providers) {
            if (provider.get()) {
                provider->sendEvent(name, parameters);
            }
        }
    }
    
    void sendEvent(const std::string &name) {
        sendEvent(name, {});
    }
    
    void trackScreenView(const std::string &name) {
        for (auto provider: providers) {
            if (provider.get()) {
                provider->trackScreenView(name);
            }
        }
    }
    
private:
    std::vector<std::shared_ptr<AnalyticsProvider>> providers;
};
