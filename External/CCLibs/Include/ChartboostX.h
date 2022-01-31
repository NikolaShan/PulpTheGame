//
//  ChartboostX.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 02/05/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#ifndef __CocosFrameworksBinding__ChartboostX__
#define __CocosFrameworksBinding__ChartboostX__

#include <string>
#include <assert.h>

#include "Advertise.h"

namespace Advertise {

class ChartboostX {
public:
    class Delegate {
    public:
        virtual ~Delegate() {}
        virtual void didLoadInterstitial(const std::string &location) {}
        virtual void didFailToLoadInterstitial(const std::string &location, unsigned int errorCode) {}
        
        virtual void didLoadMoreApps(const std::string &location) {}
        virtual void didFailToLoadMoreApps(const std::string &location, unsigned int errorCode) {}
        
        virtual void didLoadRewardedVideo(const std::string &location) {}
        virtual void didFailToLoadRewardedVideo(const std::string &location, unsigned int errorCode) {}
        virtual void didCompleteRewardedVideo(const std::string &location) = 0;
    };
    
    using Location = std::string;
  
    /*! "Default" - Supports legacy applications that only have one "Default" location */
    static const Location LocationDefault;
    
    /*! "Startup" - Initial startup of game. */
    static const Location LocationStartup;
    
    /*! "Main Menu" - Menu that provides game options. */
    static const Location LocationMainMenu;
    
    /*! "Home Screen" - Home screen the player first sees. */
    static const Location LocationHomeScreen;

    /*! "Game Screen" - Game screen where all the magic happens. */
    static const Location LocationGameScreen;
    
    /*! "Achievements" - Screen with list of achievements in the game. */
    static const Location LocationAchievements;
    
    /*! "Quests" - Quest, missions or goals screen describing things for a player to do. */
    static const Location LocationQuests;
    
    /*!  "Pause" - Pause screen. */
    static const Location LocationPause;
    
    /*! "Level Start" - Start of the level. */
    static const Location LocationLevelStart;
    
    /*! "Level Complete" - Completion of the level */
    static const Location LocationLevelComplete;
    
    /*! "Turn Complete" - Finishing a turn in a game. */
    static const Location LocationTurnComplete;
    
    /*! "IAP Store" - The store where the player pays real money for currency or items. */
    static const Location LocationIAPStore;
    
    /*! "Item Store" - The store where a player buys virtual goods. */
    static const Location LocationItemStore;
    
    /*! "Game Over" - The game over screen after a player is finished playing. */
    static const Location LocationGameOver;
    
    /*! "Leaderboard" - List of leaders in the game. */
    static const Location LocationLeaderBoard;
    
    /*! "Settings" - Screen where player can change settings such as sound. */
    static const Location LocationSettings;
    
    /*! "Quit" - Screen displayed right before the player exits a game. */
    static const Location LocationQuit;
    
    static ChartboostX *getInstance();
    
    virtual void setDelegate(Delegate *delegate) {
        this->delegate = delegate;
    }
    
    Delegate *getDelegate() const {
        return this->delegate;
    }
    
    virtual bool hasInterstitial(const Location &location) const = 0;
    virtual void loadInterstitial(const Location &location) = 0;
    virtual void presentInterstitial(const Location &location) = 0;
    
    virtual bool hasRewardedVideo(const Location &location) const = 0;
    virtual void loadRewardedVideo(const Location &location) = 0;
    virtual void presentRewardedVideo(const Location &location, std::function<void(bool success)> completion) = 0;
    
    //virtual bool hasMoreApps(const Location &location) const = 0;
    //virtual void loadMoreApps(const Location &location) = 0;
    //virtual void presentMoreApps(const Location &location) = 0;
    
    virtual void startSession(const std::string &appId, const std::string &appSignature) = 0;
    
protected:
    ChartboostX(): delegate(nullptr) { }
    virtual ~ChartboostX() { }
    
    Delegate *delegate;
};

class ChartboostInterstitial: public Advertise::Interstitial {
public:
    static std::shared_ptr<Interstitial> create(const ChartboostX::Location &location) {
        return std::static_pointer_cast<Interstitial>(std::make_shared<ChartboostInterstitial>(location));
    }
    
    ChartboostInterstitial(const ChartboostX::Location &pLocation): location(pLocation)  {
        
    }
    
    virtual ~ChartboostInterstitial() {}
    
    virtual void load() override {
        ChartboostX::getInstance()->loadInterstitial(location);
    }
    
    virtual void present() override {
        ChartboostX::getInstance()->presentInterstitial(location);
    }
    
    virtual Advertise::Status getCurrentStatus() const {
        if (ChartboostX::getInstance()->hasInterstitial(location)) {
            return Advertise::Status::LOADED;
        } else {
            return Advertise::Status::NONE;
        }
    }
    
private:
    ChartboostX::Location location;
};
    

class ChartboostRewardedVideo: public RewardedVideo {
public:
    static std::shared_ptr<RewardedVideo> create(const ChartboostX::Location &location) {
        return std::static_pointer_cast<RewardedVideo>(std::make_shared<ChartboostRewardedVideo>(location));
    }
    
    ChartboostRewardedVideo(const ChartboostX::Location &pLocation) : location(pLocation) {
        
    }
    
    virtual ~ChartboostRewardedVideo() {}
    
    virtual void load() override {
        ChartboostX::getInstance()->loadRewardedVideo(location);
    }
    
    virtual void present(std::function<void(bool success)> completion) override {
        ChartboostX::getInstance()->presentRewardedVideo(location, completion);
    }
    
    virtual Advertise::Status getCurrentStatus() const {
        if (ChartboostX::getInstance()->hasRewardedVideo(location)) {
            return Advertise::Status::LOADED;
        } else {
            return Advertise::Status::NONE;
        }
    }
    
private:
    ChartboostX::Location location;
};
    
}

#endif /* defined(__CocosFrameworksBinding__ChartboostX__) */
