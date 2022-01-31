//
//  ExpirableValueStore.h
//  Pulp
//
//  Created by Mikhail Shulepov on 26/07/15.
//
//

#pragma once

#include "base/CCValue.h"
#include "TimeUtils.h"

// Stores value that will expire after specified time

class ExpirableValueStore {
public:
    static ExpirableValueStore *getInstance();
    
    void setValue(const std::string &key, const cocos2d::Value &value, time_t liveTime);
    
    //returns Value::Null if value not exist or value was expired
    cocos2d::Value getValueForKey(const std::string &key) const;

private:
    ExpirableValueStore();
    ~ExpirableValueStore();
    
    static const std::string VALUE_KEY;
    static const std::string EXPIRATION_TIME_KEY;
    static const std::string STORAGE_FILE_NAME;
    
private:
    bool isValueExpired(const cocos2d::ValueMap &valueInfo) const;
    
    std::string getFullFilePath() const;
    void save();
    void load();
    
private:
    cocos2d::ValueMap values;
};