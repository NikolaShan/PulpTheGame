//
//  ExpirableValueStore.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 26/07/15.
//
//

#include "ExpirableValueStore.h"

#include "platform/CCFileUtils.h"
#include "Utils/FilePath.h"

USING_NS_CC;

const std::string ExpirableValueStore::VALUE_KEY = "value";
const std::string ExpirableValueStore::EXPIRATION_TIME_KEY = "expiration";
const std::string ExpirableValueStore::STORAGE_FILE_NAME = "expirable_values.plist";

ExpirableValueStore *ExpirableValueStore::getInstance() {
    static auto instance = new ExpirableValueStore;
    return instance;
}

ExpirableValueStore::ExpirableValueStore() {
    load();
}

ExpirableValueStore::~ExpirableValueStore() {
    
}

void ExpirableValueStore::setValue(const std::string &key, const Value &value, time_t liveTime) {
    auto it = values.find(key);
    cocos2d::ValueMap valueInfo;
    if (it != values.end()) {
        auto &oldValueInfo = (*it).second.asValueMap();
        if (!isValueExpired(oldValueInfo)) {
            valueInfo = oldValueInfo;
        }
    }
    
    if (valueInfo.empty()) {
        const double currentTime = static_cast<double>(time(nullptr));
        valueInfo[EXPIRATION_TIME_KEY] = Value(currentTime + liveTime);
    }
    
    valueInfo[VALUE_KEY] = value;
    values[key] = valueInfo;
    
    save();
}

//will return any value only if
Value ExpirableValueStore::getValueForKey(const std::string &key) const {
    auto it = values.find(key);
    if (it != values.end()) {
        const auto &valueInfo = (*it).second.asValueMap();
        if (!isValueExpired(valueInfo)) {
            return valueInfo.at(VALUE_KEY);
        }
    }
    
    return Value::Null;
}

bool ExpirableValueStore::isValueExpired(const cocos2d::ValueMap &valueInfo) const {
    const double expirationTime = valueInfo.at(EXPIRATION_TIME_KEY).asDouble();
    const double currentTime = static_cast<double>(time(nullptr));
    return currentTime > expirationTime;
}

std::string ExpirableValueStore::getFullFilePath() const {
    const std::string writablePath = FileUtils::getInstance()->getWritablePath();
    return FilePath::appendPathComponent(writablePath, STORAGE_FILE_NAME);
}

void ExpirableValueStore::save() {
    FileUtils::getInstance()->writeToFile(values, getFullFilePath());
}

void ExpirableValueStore::load() {
    values = FileUtils::getInstance()->getValueMapFromFile(getFullFilePath());
}