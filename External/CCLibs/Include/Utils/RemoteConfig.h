//
//  FirebaseSettings.h
//  ColorTilt
//
//  Created by Mikhail Shulepov on 22/11/2016.
//
//

#pragma once

#include <string>
#include <map>
#include <memory>
#include "base/CCValue.h"

class RemoteConfig {
public:
    RemoteConfig();
    void fetch(float expirationDurationInSeconds);
    void setDefaults(const cocos2d::ValueMap &defaults);//flat value map - values shouldn't be collections
    
    std::string getStringValue(const std::string &key) const;
    float getFloatValue(const std::string &key) const;
    int getIntValue(const std::string &key) const;
    bool getBoolValue(const std::string &key) const;
    //getDataValue(const std::string &key) const;
    
private:
    class Implementation;
    using ImplPtr = std::auto_ptr<Implementation>;
    ImplPtr pimpl;
};
