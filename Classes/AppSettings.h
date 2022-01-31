//
//  AppSettings.h
//  Square
//
//  Created by Mikhail Shulepov on 17/06/15.
//
//

#ifndef __Square__AppSettings__
#define __Square__AppSettings__

#include <string>
#include "Utils/BaseAppSettings.h"
#include "Resources/ConfigKeys.h"

class AppSettings: public BaseAppSettings {
public:
    static std::shared_ptr<AppSettings> getInstance() {
        static auto instance = std::make_shared<AppSettings>();
        return instance;
    }
    
    std::string getStringValueForKeyPath(const std::vector<std::string> &keypath) const;
    
    std::string getLeaderboardName() const {
        return getString(R::Config::GameCenter::MAIN_LEADERBOARD);
    }
    
    std::string getEndlessLeaderboardName() const {
        return getString(R::Config::GameCenter::ENDLESS_LEADERBOARD);
    }
    
    std::string getRateUrl() const {
        return getString(R::Config::RATE_URL);
    }
    
    std::string getShareUrl() const {
        return getString(R::Config::SHARE_URL);
    }
    
    std::string getVkAppId() const {
        return getString(R::Config::Social::VK_APP_ID);
    }
    
    std::string getFacebookInviteLink() const {
        return getString(R::Config::Social::FACEBOOK_INVITE_LINK);
    }
    
    std::string getAdMobInterstitialId() const {
        return getString(R::Config::Admob::INTERSTITIAL);
    }
    
    std::string getAdMobBannerId() const {
        return getString(R::Config::Admob::BANNER);
    }
    
    std::string getAdMobRewardedVideoId() const {
        return getString(R::Config::Admob::REWARDED_VIDEO);
    }
    
    // Support constants
    static const std::string ScreenshotImage;
    static const std::string CrossPromoResourceBanner;
    
    // Settings
    bool isMusicEnabled() const;
    void setMusicEnabled(bool enabled);
    
    bool isSoundsEnabled() const;
    void setSoundsEnabled(bool enabled);
    
    bool isAlreadyRated() const;
    void setGameRated();
    
    bool isColorblindMode() const;
    void setColorblindMode(bool colorblind);
    
    void setNightModeEnabled(bool nightmode);
    bool isNightModeEnabled() const;
    
    AppSettings();
    ~AppSettings() {}
    
    std::string getUiFilePath(const std::string &originalPath) const;
    
private:
    bool isVolumeWasDisabled() const;
    void updateMusic(bool enabled);
    void updateSounds(bool enabled);
};


#define UI_FILE(__FILE_PATH__) AppSettings::getInstance()->getUiFilePath(__FILE_PATH__).c_str()

#endif /* defined(__Square__AppSettings__) */
