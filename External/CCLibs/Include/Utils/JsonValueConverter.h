//
//  JsonValueConverter.h
//  Pulp
//
//  Created by Mikhail Shulepov on 30/07/15.
//
//

#pragma once

#include "base/CCValue.h"
#include "json/document.h"

namespace JsonValueConverter {
    std::string toJsonString(const cocos2d::Value &value);
    rapidjson::Document toJson(const cocos2d::Value &value);
    
    cocos2d::Value fromJsonString(const std::string &json);
    cocos2d::Value fromJson(const rapidjson::Value &json);
};