//
//  RemoteConfigStab.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 20/12/2016.
//
//

#include "Utils/RemoteConfig.h"

class RemoteConfig::Implementation {
public:
    cocos2d::ValueMap defaults;
};

RemoteConfig::RemoteConfig() {
    pimpl = ImplPtr(new Implementation());
}

void RemoteConfig::setDefaults(const cocos2d::ValueMap &defaults) {
    pimpl->defaults = defaults;
}

void RemoteConfig::fetch(float expirationDurationInSeconds) {}

std::string RemoteConfig::getStringValue(const std::string &key) const {
    const auto it = pimpl->defaults.find(key);
    if (it == pimpl->defaults.end()) {
        return "";
    }
    return (*it).second.asString();
}

float RemoteConfig::getFloatValue(const std::string &key) const {
    const auto it = pimpl->defaults.find(key);
    if (it == pimpl->defaults.end()) {
        return 0.0f;
    }
    return (*it).second.asFloat();
}

int RemoteConfig::getIntValue(const std::string &key) const {
    const auto it = pimpl->defaults.find(key);
    if (it == pimpl->defaults.end()) {
        return 0;
    }
    return (*it).second.asInt();
}

bool RemoteConfig::getBoolValue(const std::string &key) const {
    const auto it = pimpl->defaults.find(key);
    if (it == pimpl->defaults.end()) {
        return false;
    }
    return (*it).second.asBool();
}
//getDataValue(const std::string &key) const;
