//
//  BaseAppSettings.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 29/07/15.
//
//

#include "BaseAppSettings.h"
#include "Collections.h"
#include "FileUrlCache.h"
#include "FilePath.h"
#include "JsonValueConverter.h"

#include "base/CCUserDefault.h"
#include "platform/CCFileUtils.h"
#include "TimeUtils.h"
#include "StringUtils.h"

USING_NS_CC;

static const std::string FIRST_START_KEY = "PLFirstStart";

BaseAppSettings::BaseAppSettings(const std::string &defaultSettingsFilePath) {
    _isFirstStart =  UserDefault::getInstance()->getBoolForKey(FIRST_START_KEY.c_str(), true);
    if (_isFirstStart) {
        cocos2d::UserDefault::getInstance()->setBoolForKey(FIRST_START_KEY.c_str(), false);
    }
    
    loadFromFile(defaultSettingsFilePath);
}

void BaseAppSettings::loadFromFile(const std::string &filePath) {
    cocos2d::ValueMap newValues;
    if (FilePath::getExtension(filePath) == "json") {
        const std::string jsonData = FileUtils::getInstance()->getStringFromFile(filePath);
        const Value jsonValue = JsonValueConverter::fromJsonString(jsonData);
        if (jsonValue.getType() == Value::Type::MAP) {
            newValues = jsonValue.asValueMap();
        } else {
            CCLOG("Error: config should be json object");
            return;
        }
    } else {
        newValues = FileUtils::getInstance()->getValueMapFromFile(filePath);
    }
    
    const cocos2d::ValueMap defaultValues = flattenValueMap(newValues);
    remoteConfig.setDefaults(defaultValues);
#if COCOS2D_DEBUG == 1
    remoteConfig.fetch(1);
#else
    remoteConfig.fetch(TimeUtils::SECONDS_IN_DAY);
#endif
}

cocos2d::ValueMap BaseAppSettings::flattenValueMap(const cocos2d::ValueMap &valueMap) const {
    cocos2d::ValueMap ret;
    for (const auto &entry: valueMap) {
        const std::string key = entry.first;
        if (entry.second.getType() == cocos2d::Value::Type::MAP) {
            const cocos2d::ValueMap inner = flattenValueMap(entry.second.asValueMap());
            for (const auto &innerEntry: inner) {
                ret[key + "_" + innerEntry.first] = innerEntry.second;
            }
        } else if (entry.second.getType() == cocos2d::Value::Type::VECTOR){
            std::string csv;
            const ValueVector &valueVector = entry.second.asValueVector();
            bool first = true;
            for (const Value &value: valueVector) {
                if (!first) {
                    csv += ",";
                }
                csv += value.asString();
                first = false;
            }
            ret[key] = csv;
        } else {
            ret[key] = entry.second;
        }
    }
    return ret;
}

std::vector<std::string> BaseAppSettings::getStringVector(const std::string &key) const {
    const std::string stringValue = getString(key);
    return ::StringUtils::split(stringValue, ",");
}
