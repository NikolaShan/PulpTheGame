//
//  JsonLevelParser.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 20/07/15.
//
//

#include "JsonLevelParser.h"

#include "platform/CCPlatformMacros.h"
#include "json/rapidjson.h"

using namespace rapidjson;

static const float SECONDS_PER_ROTATION = 1.0f;

LevelDefinition JsonLevelParser::parse(const std::string &data) const {
    Document doc;
    doc.Parse(data.c_str());
    if (doc.HasParseError()) {
        LevelDefinition dummy;
        dummy.centreRadius = 60;
        dummy.timeout = -1.0f;
        dummy.gravity = cocos2d::Vec2(0, -150.0f);
        dummy.bulletsSpeed = std::make_shared<ConstantValue>(1.0f);
        
        std::vector<int> colors = {1, 2, 3};
        
        std::vector<OrbitDefinition> orbits;
        std::vector<Bullet> bullets;
        OrbitDefinition orbit;
        orbit.speed = std::make_shared<ConstantValue>(1.0f / SECONDS_PER_ROTATION);
        orbit.opacity = std::make_shared<ConstantValue>(1.0f);
        orbit.width = 18;
        float rotation = 0.0f;
        for (int color: colors) {
            SegmentDefinition segment;
            segment.colorIdx = color;
            segment.size = 1.0f / 3.0f;
            segment.rotation = rotation;
            rotation += segment.size;
            orbit.segments.push_back(segment);
            
            Bullet bullet;
            bullet.colorIdx = color;
            bullets.push_back(bullet);
        }
        orbits.push_back(orbit);
        dummy.orbits = orbits;
        dummy.bullets = bullets;
        cocos2d::log("PARSE ERROR");
        return dummy;
    }
    
    return parseDocument(doc);
}

LevelDefinition JsonLevelParser::parseDocument(const Document &doc) const {
    LevelDefinition ret;
    
    ret.orbits = parseOrbits(doc["orbits"]);
    ret.bullets = parseBullets(doc["bullets"]);
    ret.centreRadius = getFloatOr(doc, "radius", 80.0f);
    ret.timeout = getFloatOr(doc, "timeout", -1.0f);
    if (doc.HasMember("hint")) {
        ret.hint = doc["hint"].GetString();
    }
    if (doc.HasMember("bullets_speed")) {
        ret.bulletsSpeed = getDynamicValue(doc["bullets_speed"]);
    }
    if (!ret.bulletsSpeed) {
        ret.bulletsSpeed = std::make_shared<ConstantValue>(1.0f);
    }
    
    GameType gameType = GameType::BULLETS_CONTROL;
    if (doc.HasMember("type")) {
        const std::string typeName = doc["type"].GetString();
        if (typeName  == "bullets") {
            gameType = GameType::BULLETS_CONTROL;
        } else if (typeName == "rings") {
            gameType = GameType::RINGS_CONGROL;
        }
    }
    
    ret.type = gameType;
    if (gameType != GameType::RINGS_CONGROL) {
        ret.gravity.x = 0.0f;
        ret.gravity.y = - getFloatOr(doc, "gravity", 150.0f);
    } else {
        ret.gravity = cocos2d::Vec2::ZERO;
    }
    
    ret.bulletsAutoLaunchInterval = getFloatOr(doc, "auto_launch_interval", 1.0f);
    
    CCASSERT(segmentsCount == ret.bullets.size(), "Count of segments != count of bullets");
    CCASSERT(segmentsColorSum == bulletsColorSum, "Segments colors != bullets colors");
    
    return ret;
}

std::vector<OrbitDefinition> JsonLevelParser::parseOrbits(const Value &info) const {
    CCASSERT(info.IsArray(), "Warning: no orbits provided or value not an array");
    segmentsCount = 0;
    segmentsColorSum = 0;
    
    std::vector<OrbitDefinition> ret;
    for (unsigned int i = 0; i < info.Size(); ++i) {
        ret.push_back(parseOrbit(info[i]));
    }
    return ret;
}

OrbitDefinition JsonLevelParser::parseOrbit(const Value &info) const {
    OrbitDefinition ret;
    ret.segments = parseSegments(info["segments"]);
    ret.initialRotation = getFloatOr(info, "rotation", 0.0f);
    ret.width = getFloatOr(info, "width", 20.0f);
    
    const float speedScale = 1.0f / SECONDS_PER_ROTATION;
    if (info.HasMember("speed")) {
        auto &value = info["speed"];
        if (value.IsNumber()) {
            ret.speed = std::make_shared<ConstantValue>(speedScale * value.GetDouble());
            
        } else if (value.IsArray()) {
            auto values = jsonArrayToFloatVector(value);
            for (int i = 0; i < values.size(); ++i) {
                values[i] *= speedScale;
            }
            ret.speed = std::make_shared<QueryDependedValue>(values);
            
        } else  if (value.IsObject()) {
            auto values = jsonArrayToFloatVector( value["values"] );
            for (int i = 0; i < values.size(); ++i) {
                values[i] *= speedScale;
            }
            
            std::vector<float> durations;
            if (value.HasMember("turns")) {
                durations = jsonArrayToFloatVector( value["turns"] );
                for (int i = 0; i < durations.size(); ++i) {
                    const float speed = fabsf(values[i]);
                    if (speed > FLT_EPSILON) {
                        durations[i] /= speed;
                    }
                }
                
            } else {
                durations = jsonArrayToFloatVector( value["durations"] );
            }
            
            ret.speed = std::make_shared<TimeDependedValue>(values, durations);
        }
    }
    
    if (info.HasMember("fade")) {
        ret.opacity = getDynamicValue(info["fade"]);
    } else {
        ret.opacity = std::make_shared<ConstantValue>(1.0f);
    }
    
    return ret;
}

std::vector<SegmentDefinition> JsonLevelParser::parseSegments(const Value &info) const {
    CCASSERT(info.IsArray(), "segments must be of Array type");
    float remainingSize = 1.0f;
    std::vector<SegmentDefinition> segments;
    for (unsigned int i = 0; i < info.Size(); ++i) {
        SegmentDefinition segment = parseSegment(info[i], remainingSize);
        segments.push_back(segment);
        remainingSize -= segment.size;
        CCASSERT(remainingSize > -FLT_EPSILON, "Total size of segments > 1");
        segmentsCount++;
        segmentsColorSum += segment.colorIdx;
    }
    return segments;
}

SegmentDefinition JsonLevelParser::parseSegment(const Value &info, float remainingSize) const {
    SegmentDefinition segment;
    segment.size = getFloatOr(info, "size", remainingSize);
    segment.colorIdx = info["color"].GetInt();
    segment.rotation = getFloatOr(info, "rotation", -2.0f);
    return segment;
}

std::vector<Bullet> JsonLevelParser::parseBullets(const Value &info) const {
    CCASSERT(info.IsArray(), "Bullets must be an array");
    bulletsColorSum = 0;
    std::vector<Bullet> bullets;
    for (unsigned int i = 0; i < info.Size(); ++i) {
        Bullet bullet = parseBullet(info[i]);
        bullets.push_back(bullet);
        bulletsColorSum += bullet.colorIdx;
    }
    return bullets;
}

Bullet JsonLevelParser::parseBullet(const Value &info) const {
    Bullet ret;
    ret.colorIdx = info.GetInt();
    return ret;
}

float JsonLevelParser::getFloatOr(const Value &parent, const std::string &key, float defaultValue) const {
    if (parent.HasMember(key.c_str())) {
        const auto &value = parent[key.c_str()];
        if (value.IsNumber()) {
            return static_cast<float>(value.GetDouble());
        }
    }

    return defaultValue;
}

bool JsonLevelParser::getBoolOr(const Value &parent, const std::string &key, bool defaultValue) const {
    if (parent.HasMember(key.c_str())) {
        const auto &value = parent[key.c_str()];
        if (value.IsBool()) {
            return value.GetBool();
        }
    }
    
    return defaultValue;
}

std::shared_ptr<DynamicValue> JsonLevelParser::getDynamicValue(const rapidjson::Value &value) const {
    if (value.IsNumber()) {
        auto ret = std::make_shared<ConstantValue>(static_cast<float>(value.GetDouble()));
        return std::dynamic_pointer_cast<DynamicValue>(ret);
    }
    if (value.IsArray()) {
        auto values = jsonArrayToFloatVector(value);
        auto ret = std::make_shared<QueryDependedValue>(values);
        return std::dynamic_pointer_cast<DynamicValue>(ret);
    }
    if (value.IsObject()) {
        auto values = jsonArrayToFloatVector( value["values"] );
        auto durations = jsonArrayToFloatVector( value["durations"] );
        auto ret = std::make_shared<TimeDependedValue>(values, durations);
        return std::dynamic_pointer_cast<DynamicValue>(ret);
    }
    return nullptr;
}

std::vector<float> JsonLevelParser::jsonArrayToFloatVector(const Value &value) const {
    std::vector<float> values;
    for (unsigned int i = 0; i < value.Size(); ++i) {
        values.push_back(value[i].GetDouble());
    }
    return values;
}