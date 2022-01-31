//
//  RateScheduler.hpp
//  Pulp
//
//  Created by Mikhail Shulepov on 16.10.15.
//
//

#pragma once

#include <time.h>
#include <string>

#include "platform/CCPlatformMacros.h"

class RateScheduler {
public:
    static RateScheduler *getInstance();
    
    CC_SYNTHESIZE(int, usesUntilPrompt, UsesUntilPrompt);
    CC_SYNTHESIZE(int, eventsUntilPrompt, EventsUntilPrompt)
    CC_SYNTHESIZE(double, daysUntilPrompt, DaysUntilPrompt);
    CC_SYNTHESIZE(double, remindPeriod, RemindPeriod); //in days
    CC_SYNTHESIZE(double, minIntervalFromLaunch, MinIntervalFromLaunch); //in seconds
  
    CC_SYNTHESIZE(bool, promptForNewVersionIfUserRated, PromptForNewVersionIfUserRated);
    
    void incrementUsesCount();
    int getUsesCount() const;
    
    void incrementEventsCount();
    void setEventsCount(int count);
    int getEventsCount() const;
    
    bool declinedThisVersion() const;
    bool declinedAnyVersion() const;
    
    bool ratedThisVersion() const;
    bool ratedAnyVersion() const;
    
    time_t getInstallTime() const;
    time_t getLastPromptTime() const;
    
    bool shouldPromptForRate() const;
    
    void setRatedThisVersion();
    void setDeclinedThisVersion();
    void setJustPromptedForRate();
    
private:
    RateScheduler();
    std::string getDeclinedVersionName() const;
    std::string getRatedVersionName() const;
    
private:
    static const std::string USES_COUNT_KEY;
    static const std::string EVENTS_COUNT_KEY;
    static const std::string DECLINED_VERSION_KEY;
    static const std::string RATED_VERSION_KEY;
    static const std::string INSTALL_TIME_KEY;
    static const std::string LAST_PROMPT_TIME_KEY;
    
    time_t launchTime;
};
