//
// Created by Mike on 26.04.14.
//

#include <assert.h>
#include "Utils/Collections.h"
#include "Utils/StringUtils.h"

USING_NS_CC;

ValueMap &Collections::getOrInsertValueMap(ValueMap &src, const std::string &key) {
    auto it = src.find(key);
    if (it == src.end()) {
        src[key] = Value(ValueMap());
        return src[key].asValueMap();
    } else {
        return (*it).second.asValueMap();
    }
}

void Collections::mergeValueMap(cocos2d::ValueMap &to, const cocos2d::ValueMap &from) {
    for (auto &entry : from) {
        auto old = to.find(entry.first);
        if (old == to.end() || (*old).second.getType() != Value::Type::MAP) {
            to[entry.first] = entry.second;
        } else {
            mergeValueMap((*old).second.asValueMap(), entry.second.asValueMap());
        }
    }
}

bool Collections::valueMapGetBool(const cocos2d::ValueMap &map, const std::string &key, bool defaultValue) {
    auto it = map.find(key);
    if (it == map.end()) {
        return defaultValue;
    }
    return (*it).second.asBool();
}

int Collections::valueMapGetInt(const cocos2d::ValueMap &map, const std::string &key, int defaultValue) {
    auto it = map.find(key);
    if (it == map.end()) {
        return defaultValue;
    }
    return (*it).second.asInt();
}

std::string Collections::valueMapGetString(const cocos2d::ValueMap &map, const std::string &key) {
    auto it = map.find(key);
    assert(it != map.end());
    if (it == map.end()) {
        return "";
    }
    return (*it).second.asString();
}

std::string Collections::valueMapGetString(const ValueMap &map, const std::string &key, const std::string &defaultValue) {
    auto it = map.find(key);
    if (it == map.end()) {
        return defaultValue;
    }
    return (*it).second.asString();
}

std::vector<std::string> Collections::valueVectorToStringVector(const cocos2d::ValueVector &vv) {
    std::vector<std::string> ret;
    for (auto &value : vv) {
        ret.push_back(value.asString());
    }
    return ret;
}

#pragma mark -
#pragma mark Value for key path

const cocos2d::Value &Collections::getValueForKeyPath(const ValueMap &target, const std::string &keyPath) {
    auto pathComponents = ::StringUtils::split(keyPath, "/");
    return getValueForKeyPath(target, pathComponents);
}

const cocos2d::Value &Collections::getValueForKeyPath(const ValueMap &target, const std::vector<std::string> &keyPath) {
    if (!keyPath.size()) {
        return Value::Null;
    }
    
    const std::string nextPathComponent = keyPath[0];
    if (target.find(nextPathComponent) != target.end()) {
        const Value &nextValue = target.at(nextPathComponent);
        std::vector<std::string> nextPathComponents(++keyPath.begin(), keyPath.end());
        if (nextPathComponents.size()) {
            if (nextValue.getType() == Value::Type::MAP) {
                return getValueForKeyPath(nextValue.asValueMap(), nextPathComponents);
            } else {
                //CCLOG("KEYPATH WORKS ONLY WITH MAPS");
            }
        } else {
            return nextValue;
        }
    }
    
    return Value::Null;
}