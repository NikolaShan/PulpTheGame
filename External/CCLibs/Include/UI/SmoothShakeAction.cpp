//
//  SmoothShake.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 22/02/16.
//
//

#include "SmoothShakeAction.h"
#include "Utils/Noise.h"

USING_NS_CC;

///////////////////////////////////////////////////////////////////////////////
// CShakeAction
///////////////////////////////////////////////////////////////////////////////
SmoothShakeAction::SmoothShakeAction(): _speed(0.0f), _magnitude(0.0f), _randomStart(0.0f) {

}

SmoothShakeAction *SmoothShakeAction::create(float duration, float speed, float magnitude) {
    SmoothShakeAction *ret = new (std::nothrow) SmoothShakeAction();
    
    if (ret) {
        if (ret->initWithDuration(duration, speed, magnitude)) {
            ret->autorelease();
        } else {
            delete ret;
            ret = nullptr;
        }
    }
    
    return ret;
}

bool SmoothShakeAction::initWithDuration(float duration, float speed, float magnitude) {
    bool ret = false;
    
    if (ActionInterval::initWithDuration(duration)) {
        _speed = speed;
        _magnitude = magnitude;
        _randomStart = cocos2d::random(-1000.0f, 1000.0f);
        ret = true;
    }
    
    return ret;
}

void SmoothShakeAction::update(float percent) {
    if (_target == nullptr)
        return;
    
    float damper = 1.0f - clampf(2.0f * percent - 1.0f, 0.0f, 1.0f);
    
    float alphaX = _randomStart + _speed * percent;
    float alphaY = (_randomStart + 1000.0f) + _speed * percent;
    
    // noise1 output range: -0.5 ~ 0.5
    float x = Noise::perlinNoise1D(alphaX) * 2.0f;    // mapping -1.0 ~ 1.0
    float y = Noise::perlinNoise1D(alphaY) * 2.0f;
    
    x *= (_magnitude * damper);
    y *= (_magnitude * damper);
    
    Mat4 mat;
    mat.m[12] = x;
    mat.m[13] = y;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
    
    _target->setAdditionalTransform(&mat);
}

void SmoothShakeAction::stop() {
    if (_target)
        _target->setAdditionalTransform(nullptr);
    ActionInterval::stop();
}