//
//  GameManager.h
//  Pulp
//
//  Created by Mikhail Shulepov on 20/07/15.
//
//

#pragma once

#include "LevelDefinition.h"

struct SegmentColorInfo {
    cocos2d::Color4B mainColor;
    cocos2d::Color4B shadowColor;
};

class GameManager {
public:
    static GameManager *getInstance();
    
    int getCurrentLevelNumber() const;
    void setCurrentLevel(int number);
    void startNextLevel() {
        setCurrentLevel(getCurrentLevelNumber() + 1);
    }
    
    void setLevelCompleted(int number);
    int getCompletedLevelsCount() const;
    bool canPlayLevel(int number) const;
    
    LevelDefinition getLevel(int number) const;
    LevelDefinition getCurrentLevel() const {
        recentlyPlayedLevelNumber = getCurrentLevelNumber();
        return getLevel(recentlyPlayedLevelNumber);
    }
    LevelDefinition getRandomLevel() const;
    
    int getRecentlyPlayedLevelNumber() const {
        return recentlyPlayedLevelNumber;
    }
    
    int getEndlessModeHighscore() const;
    void setEndlessModeScore(int score);
    
    int getLevelsCount() const;
    
    const std::vector<SegmentColorInfo> &getColorScheme() const {
        return colorScheme;
    }
    
    void incrementAttemptsForCurrentLevel();
    int getAttemptsCountForCurrentLevel() const;
    
    int getAvailableLevelSkips() const;
    void setAvailableLevelSkips(int count);
    
private:
    GameManager();
    void remapLevelsFirst();
    
private:
    std::vector<SegmentColorInfo> colorScheme;
    std::vector<int> levelsRemap;
    mutable int recentlyPlayedLevelNumber = 0;
};