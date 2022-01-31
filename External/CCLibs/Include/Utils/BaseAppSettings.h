//
//  BaseAppSettings.h
//  Pulp
//
//  Created by Mikhail Shulepov on 29/07/15.
//
//

#pragma once

#include <vector>
#include "base/CCValue.h"
#include "RemoteConfig.h"


class BaseAppSettings {
public:
    BaseAppSettings(const std::string &defaultSettingsFilePath);

    // Support
    std::string getString(const std::string &key) const {
        return remoteConfig.getStringValue(key);
    }
    
    int getInt(const std::string &key) const {
        return remoteConfig.getIntValue(key);
    }
    
    float getFloat(const std::string &key) const {
        return remoteConfig.getFloatValue(key);
    }
    
    bool getBool(const std::string &key) const {
        return remoteConfig.getBoolValue(key);
    }
    
    bool isFirstStart() const {
        return _isFirstStart;
    }
    
    //comma separated values -> array of Values of string type
    std::vector<std::string> getStringVector(const std::string &key) const;
    
    std::vector<std::string> getKeysWithPrefix(const std::string &prefix) const;
    
private:
    void loadFromFile(const std::string &filePath);
    cocos2d::ValueMap flattenValueMap(const cocos2d::ValueMap &valueMap) const;

private:
    bool _isFirstStart;
    RemoteConfig remoteConfig;
};
