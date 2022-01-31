//
//  TuneAnalytics.h
//  Pulp
//
//  Created by Mikhail Shulepov on 10/09/15.
//
//

#pragma once

#include <memory>
#include <string>

class TuneAnalytics {
public:
    static std::shared_ptr<TuneAnalytics> getInstance();
    
    virtual void start(const std::string &advertiserId, const std::string &conversionKey, bool existingUser) = 0;
    
    virtual void measureLevelAchieved(int levelNumber) = 0;
    
private:
};