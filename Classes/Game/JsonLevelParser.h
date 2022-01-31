//
//  JsonLevelParser.h
//  Pulp
//
//  Created by Mikhail Shulepov on 20/07/15.
//
//

#pragma once

#include "LevelDefinition.h"

#include "json/document.h"
#include "DynamicValue.h"

class JsonLevelParser {
public:
    LevelDefinition parse(const std::string &data) const;
    LevelDefinition parseDocument(const rapidjson::Document &doc) const;    
    
    std::vector<OrbitDefinition> parseOrbits(const rapidjson::Value &info) const;
    OrbitDefinition parseOrbit(const rapidjson::Value &info) const;

    std::vector<SegmentDefinition> parseSegments(const rapidjson::Value &info) const;
    SegmentDefinition parseSegment(const rapidjson::Value &info, float remainingSize) const;
    
    std::vector<Bullet> parseBullets(const rapidjson::Value &info) const;
    Bullet parseBullet(const rapidjson::Value &info) const;
    
private:
    float getFloatOr(const rapidjson::Value &value, const std::string &key, float defaultValue) const;
    bool getBoolOr(const rapidjson::Value &value, const std::string &key, bool defaultValue) const;
    cocos2d::Color4B getColor(const rapidjson::Value &info) const;
    std::shared_ptr<DynamicValue> getDynamicValue(const rapidjson::Value &value) const;
    
    std::vector<float> jsonArrayToFloatVector(const rapidjson::Value &value) const;
    
private:
    // Just for validation
    mutable int segmentsCount;
    mutable int segmentsColorSum;
    mutable int bulletsColorSum;
};