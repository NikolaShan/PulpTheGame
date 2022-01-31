//
//  SecondChanceScheduler.hpp
//  Pulp
//
//  Created by Mikhail Shulepov on 21.11.15.
//
//

#pragma once

#include "Game/LevelDefinition.h"
#include "base/CCValue.h"

class SecondChanceScheduler {
public:
    static SecondChanceScheduler *getInstance() {
        static auto instance = new SecondChanceScheduler;
        return instance;
    }
    
    SecondChanceScheduler();
    
    void didStartLevel(const LevelDefinition &level);
    
    void didShow();
    void didDecline();
    void didAccept();
    
    bool didFailAt(size_t remainingCount);

private:   
    void resetRequiredDefeatsCount();
    
private:
    int lastPlayedLevel = -1;
    bool wasUsedAtCurrentAttempt = false;
    int initialLevelBullets = 0;
    std::vector<size_t> failuresDistribution;
    
    size_t requiredDefeatsBeforeShowing = 0;
    int declinesCount = 0;
};
