//
//  GameCenter.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 09/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#ifndef __Planemo__GameCentre__
#define __Planemo__GameCentre__

#include <functional>
#include <memory>
#include "Utils/AppUtils.h"

namespace GameKit {

class GameCenter {
public:
    static GameCenter *getInstance();
    
    virtual void start() = 0;
    
    //Settings
    virtual void setShowCompletionBanner(bool showCompletionBanner) = 0;
    
    // Auth
    //will be called every time player authentication state changed (first time called immediately)
    virtual void removeAuthCallback(void *targetPtr) {
        const uintptr_t idx = reinterpret_cast<uintptr_t>(targetPtr);
        authCallbacks.erase(idx);
    }
    virtual void addAuthCallback(std::function<void(bool authenticated)> callback, void *targetPtr) {
        callback(isLocalPlayerAuthenticated());
        const uintptr_t idx = reinterpret_cast<uintptr_t>(targetPtr);
        authCallbacks[idx] = callback;
    };
    
    virtual void authenticateLocalPlayer(std::function<void(bool success)> callback) = 0;
    virtual void signOut() = 0;
    virtual bool isLocalPlayerAuthenticated() const = 0;
  
    // Scores
    void submitScore(int64_t s, const std::string &leaderboardName) {
        impl_submitScore(s, leaderboardName);
    }

    // Achivements
    void unlockAchievement(const std::string &name) {
        reportAchievement(name, -1, -1);
    }
    
    void reportAchievement(const std::string &name, int stepsDone, int stepsRequired) {
        impl_reportAchievements({name}, {stepsDone}, {stepsRequired});
    }
    
    void reportAchievements(const std::vector<std::string> &names, const std::vector<int> &stepsDone, const std::vector<int> &stepsRequired) {
        impl_reportAchievements(names, stepsDone, stepsRequired);
    }
    
    // Game Center Views
    void showLeaderBoard(const std::string &name) {
        if (isLocalPlayerAuthenticated()) {
            impl_showLeaderBoard(name);
        } else {
            authenticateLocalPlayer([this, name](bool) {
                impl_showLeaderBoard(name);
            });
        }
    }
    
    virtual void showAchievements() = 0;
    
    virtual void getLocalPlayerScoreForLeaderboard(const std::string &leaderboardName, std::function<void(int highscore)> result) = 0;
    
    //delegate
    //void setDelegate(std::shared_ptr<GameCenterDelegate> gkhDelegate);
    
protected:
    virtual void impl_submitScore(int64_t s, const std::string &leaderboardName) = 0;
    virtual void impl_reportAchievements(const std::vector<std::string> &identifiers, const std::vector<int> &stepsDone, const std::vector<int> &stepsRequired) = 0;
    virtual void impl_showLeaderBoard(const std::string &identifier) = 0;
    void onAuthStateChanged(bool authenticated) {
        for (auto &&entry: authCallbacks) {
            entry.second(authenticated);
        }
    }
    
    GameCenter() {}
    virtual ~GameCenter() {}
    
    std::map<uintptr_t, std::function<void(bool)>> authCallbacks;
};

}

#endif /* defined(__Planemo__GameCentre__) */
