//
//  LevelDefinition.h
//  Pulp
//
//  Created by Mikhail Shulepov on 20/07/15.
//
//

#pragma once

#include <vector>

#include "base/ccTypes.h"
#include "DynamicValue.h"


struct SegmentDefinition {
public:
    float size; //from 0.0 to 1.0
    float rotation = -1.0f;
    int colorIdx;
};

struct OrbitDefinition {
public:
    DynamicValuePtr speed; //number of full rotations per second
    
    std::vector<SegmentDefinition> segments;
    float initialRotation; // initial rotation
    float width;
    
    DynamicValuePtr opacity;
};

struct Bullet {
public:
    int colorIdx;
};

enum class GameType {
    BULLETS_CONTROL,
    RINGS_CONGROL
};

class LevelDefinition {
public:
    GameType type = GameType::BULLETS_CONTROL;
    
    float centreRadius;    // radius of cental circle
    std::vector<OrbitDefinition> orbits;
    std::vector<Bullet> bullets;
    DynamicValuePtr bulletsSpeed;
    
    //misc
    float bulletsAutoLaunchInterval = 1.0f;//for GameType::RINGS_CONTROL
    std::string hint;
    int number;
    float timeout;
    cocos2d::Vec2 gravity;
    
};