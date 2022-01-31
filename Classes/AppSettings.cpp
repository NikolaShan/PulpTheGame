//
//  AppSettings.cpp
//  Square
//
//  Created by Mikhail Shulepov on 17/06/15.
//
//

#include "AppSettings.h"

#include "base/CCUserDefault.h"
#include "platform/CCPlatformConfig.h"
#include "Resources/Resources.h"
#include "Utils/AppUtils.h"
#include "Utils/PLAudioEngine.h"
#include "Utils/StringUtils.h"

USING_NS_CC;

static const std::string VOLUME_ENABLED_KEY = "PLVolumeEnabled";
static const std::string ALREADY_RATED = "PLAlreadyRated";
static const std::string COLORBLIND_MODE = "ColorblindMode";
static const std::string MUSIC_ENABLED_KEY = "PLMusicEnabled";
static const std::string SOUNDS_ENABLED_KEY = "PLSoundsEnabled";
static const std::string NIGHT_MODE_ENABLED_KEY = "PLNightModeEnabled";

AppSettings::AppSettings(): BaseAppSettings(
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
   R::APP_SETTINGS_IOS
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
   R::APP_SETTINGS_ANDROID
#endif
) {
    if (isVolumeWasDisabled()) {
        setMusicEnabled(false);
        setSoundsEnabled(false);
    } else {
        updateMusic(isMusicEnabled());
        updateSounds(isSoundsEnabled());
    }
}

const std::string AppSettings::ScreenshotImage = "screenshot.png";
const std::string AppSettings::CrossPromoResourceBanner = "banner.png";

bool AppSettings::isVolumeWasDisabled() const {
    bool enabled = UserDefault::getInstance()->getBoolForKey(VOLUME_ENABLED_KEY.c_str(), true);
    UserDefault::getInstance()->setBoolForKey(VOLUME_ENABLED_KEY.c_str(), true);
    return !enabled;
}

bool AppSettings::isMusicEnabled() const {
    return UserDefault::getInstance()->getBoolForKey(MUSIC_ENABLED_KEY.c_str(), true);
}

void AppSettings::setMusicEnabled(bool enabled) {
    UserDefault::getInstance()->setBoolForKey(MUSIC_ENABLED_KEY.c_str(), enabled);
    updateMusic(enabled);
}

bool AppSettings::isSoundsEnabled() const {
    return UserDefault::getInstance()->getBoolForKey(SOUNDS_ENABLED_KEY.c_str(), true);
}

void AppSettings::setSoundsEnabled(bool enabled) {
    UserDefault::getInstance()->setBoolForKey(SOUNDS_ENABLED_KEY.c_str(), enabled);
    updateSounds(enabled);
}

bool AppSettings::isAlreadyRated() const {
    return UserDefault::getInstance()->getBoolForKey(ALREADY_RATED.c_str(), false);
}

void AppSettings::setGameRated() {
    UserDefault::getInstance()->setBoolForKey(ALREADY_RATED.c_str(), true);
}

bool AppSettings::isColorblindMode() const {
    return UserDefault::getInstance()->getBoolForKey(COLORBLIND_MODE.c_str(), false);
}

void AppSettings::setColorblindMode(bool colorblind) {
    UserDefault::getInstance()->setBoolForKey(COLORBLIND_MODE.c_str(), colorblind);
}

void AppSettings::updateMusic(bool enabled) {
    auto audioEngine = PLAudioEngine::getInstance();
    if (enabled) {
        audioEngine->setBackgroundMusicVolume(0.075f);
    } else {
        audioEngine->setBackgroundMusicVolume(0.0f);
    }
}

void AppSettings::updateSounds(bool enabled) {
    auto audioEngine = PLAudioEngine::getInstance();
    if (enabled) {
        audioEngine->setEffectsVolume(0.45f);
    } else {
        audioEngine->setEffectsVolume(0.0f);
    }
}

std::string AppSettings::getStringValueForKeyPath(const std::vector<std::string> &keypath) const {
    const std::string strPath = ::StringUtils::join(keypath, "_");
    return getString(strPath);
}

void AppSettings::setNightModeEnabled(bool nightmode) {
    UserDefault::getInstance()->setBoolForKey(NIGHT_MODE_ENABLED_KEY.c_str(), nightmode);
}

bool AppSettings::isNightModeEnabled() const {
    return UserDefault::getInstance()->getBoolForKey(NIGHT_MODE_ENABLED_KEY.c_str(), false);
}

std::string AppSettings::getUiFilePath(const std::string &originalPath) const {
    if (isNightModeEnabled()) {
        std::string ret = originalPath;
        const std::string from = "UI/";
        const size_t pos = ret.find(from);
        if (pos != std::string::npos) {
            ret.replace(pos, from.length(), "UiNight/");
        }
        return ret;
    } else {
        return originalPath;
    }
}

