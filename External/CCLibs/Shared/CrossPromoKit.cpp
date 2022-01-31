//
//  CrossPromoKit.cpp
//  CCLibs
//
//  Created by Mikhail Shulepov on 15/06/15.
//
//

#include "base/CCUserDefault.h"
#include "platform/CCFileUtils.h"

#include "Utils/FileUrlCache.h"
#include "Utils/FilePath.h"
#include "Utils/Collections.h"
#include "Utils/AppUtils.h"
#include "Utils/Localization.h"
#include "Utils/JsonValueConverter.h"

#include "CrossPromoKit.h"

USING_NS_CC;

CrossPromoKit::CrossPromoKit() {
    
}

CrossPromoKit::~CrossPromoKit() {
    
}

void CrossPromoKit::startWithBaseUrl(const std::string &url, std::shared_ptr<Rewarder> prewarder) {
    rewarder = prewarder;
    baseUrl = url;
    loadCachedAppInfos();
    downloadAppInfos();
}

void CrossPromoKit::loadCachedAppInfos() {
    const std::string cachePath = getFullPathForCacheFile();
    cocos2d::ValueVector cached;
    if (FilePath::getExtension(cachePath) == "json") {
        const std::string jsonData = FileUtils::getInstance()->getStringFromFile(cachePath);
        auto jsonValue = JsonValueConverter::fromJsonString(jsonData);
        if (jsonValue.getType() == Value::Type::VECTOR) {
            cached = jsonValue.asValueVector();
        }
    } else {
        cached = FileUtils::getInstance()->getValueVectorFromFile(cachePath);
    }
    if (cached.empty()) {
        return; //early return
    }
    parseData(cached);
}

void CrossPromoKit::downloadAppInfos() {
    FileUrlCache::getInstance()->cacheDataAtUrl(getInfoUrl(), [this](const FileUrlCache::CacheResult &result) {
        if (result.success) {
            this->loadCachedAppInfos();
        }
    }, 0);
}

void CrossPromoKit::parseData(const cocos2d::ValueVector &vec) {
    std::vector<AppInfoPtr> appInfos;
    for (auto &entry: vec) {
        if (entry.getType() == Value::Type::MAP) {
            auto app = std::make_shared<AppInfo>(entry.asValueMap(), baseUrl);
            if (app->isValid()) {
                appInfos.push_back(app);
            }
        }
    }
    this->apps = appInfos;
    
    // Don't give rewards for apps that were installed on first app launch
    static const std::string FirstLaunchKey = "CrPrFirstCheck";
    const bool firstLaunch = UserDefault::getInstance()->getBoolForKey(FirstLaunchKey.c_str(), true);
    if (firstLaunch) {
        for (auto app: apps) {
            if (app->isInstalled()) {
                app->markRewarded();
            }
        }
        UserDefault::getInstance()->setBoolForKey(FirstLaunchKey.c_str(), false);
    }
}

std::string CrossPromoKit::getInfoUrl() const {
    return FilePath::appendPathComponent(baseUrl, "info.json");
}

std::string CrossPromoKit::getFullPathForCacheFile() const {
    return FileUrlCache::getInstance()->getFileCacheForUrl(getInfoUrl());
}

void CrossPromoKit::makeRewards() {
    std::vector<AppInfoPtr> appsToReward;
    for (auto app: apps) {
        if (app->isInstalled() && !app->wasRewarded()) {
            appsToReward.push_back(app);
            app->markRewarded();
        }
    }
    if (rewarder) {
        rewarder->makeRewardsForInstallingCrossPromoApps(appsToReward);
    }
}

std::vector<CrossPromoKit::AppInfoPtr> CrossPromoKit::getNotInstalledApps() const {
    std::vector<AppInfoPtr> output;
    for (auto app: apps) {
        if (!app->isInstalled() && !app->wasRewarded()) {
            output.push_back(app);
        }
    }
    return output;
}

CrossPromoKit::AppInfoPtr CrossPromoKit::getRandomNotInstalledApp() const {
    auto apps = getNotInstalledApps();
    if (!apps.empty()) {
        std::srand(static_cast<unsigned>(time(nullptr)));
        return apps[static_cast<size_t>(std::rand()) % apps.size()];
    }
    return nullptr;
}


#pragma mark -
#pragma mark App Info

const std::string CrossPromoKit::AppInfo::KEY_NAME = "name";
const std::string CrossPromoKit::AppInfo::KEY_DESCRIPTION = "about";
const std::string CrossPromoKit::AppInfo::KEY_APP_ID = "app_id";
const std::string CrossPromoKit::AppInfo::KEY_URL_SCHEMES = "ios_url_schemes";
const std::string CrossPromoKit::AppInfo::KEY_PROVIDER_TOKEN = "provider_token";
const std::string CrossPromoKit::AppInfo::KEY_PACKAGE_NAME = "android_package_name";
const std::string CrossPromoKit::AppInfo::KEY_ENTRY_ID = "entry_id";
const std::string CrossPromoKit::AppInfo::KEY_ANDROID_STORE_URL = "android_store_url";

CrossPromoKit::AppInfo::AppInfo(const cocos2d::ValueMap &pinfo, const std::string &pbaseUrl)
: info(pinfo)
, baseUrl(pbaseUrl) {
    
}

std::string CrossPromoKit::AppInfo::getName() const {
    return getLocalizedValue(KEY_NAME, "Noname");
}

std::string CrossPromoKit::AppInfo::getDesciption() const {
    return getLocalizedValue(KEY_DESCRIPTION, "No description");
}

std::string CrossPromoKit::AppInfo::getLocalizedValue(const std::string &key, const std::string &defaultValue) const {
    auto lang = Localization::getInstance()->getLanguage();
    auto it = info.find(key + "_" + lang);
    if (it != info.end()) {
        return (*it).second.asString();
    }
    auto defIt = info.find(key);
    if (defIt != info.end()) {
        return (*defIt).second.asString();
    }
    return defaultValue;
}

std::string CrossPromoKit::AppInfo::getEntryIdentifier() const {
    return Collections::valueMapGetString(info, KEY_ENTRY_ID, "");
}

std::string CrossPromoKit::AppInfo::getUrlForResource(const std::string &name) const {
    std::string fullResourcePath = FilePath::appendPathComponent(baseUrl, getEntryIdentifier());
    return FilePath::appendPathComponent(fullResourcePath, name);
}

void CrossPromoKit::AppInfo::markRewarded() {
    UserDefault::getInstance()->setBoolForKey(getEntryIdentifier().c_str(), true);
}

void CrossPromoKit::AppInfo::showInAppStore() {
    CrossPromoKit::getInstance()->showInAppStore(shared_from_this());
}

bool CrossPromoKit::AppInfo::wasRewarded() const {
    return UserDefault::getInstance()->getBoolForKey(getEntryIdentifier().c_str());
}

bool CrossPromoKit::AppInfo::isValid() const {
    for (auto field: getRequiredFields()) {
        if (info.find(field) == info.end()) {
            cocos2d::log("Warning: find incorrect definition for app in CrossPromoKit: %s", getName().c_str());
            return false;
        }
    }
    return true;
}

void CrossPromoKit::AppInfo::downloadResource(const std::string &name) {
    FileUrlCache::getInstance()->cacheDataAtUrl(getUrlForResource(name), nullptr);
}

bool CrossPromoKit::AppInfo::isResourceDownloaded(const std::string &name) const {
    return FileUrlCache::getInstance()->hasCachedDataForUrl(getUrlForResource(name));
}

std::string CrossPromoKit::AppInfo::getResourceLocalPath(const std::string &name) const {
    return FileUrlCache::getInstance()->getFileCacheForUrl(getUrlForResource(name));
}
