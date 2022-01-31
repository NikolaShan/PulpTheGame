//
//  SmoothShake.hpp
//  Pulp
//
//  Created by Mikhail Shulepov on 22/02/16.
//
//

#pragma once

#include "2d/CCNode.h"
#include "2d/CCActionInterval.h"

/// shake action using perlin noise 1d
class SmoothShakeAction: public cocos2d::ActionInterval {
public:
    static SmoothShakeAction *create(float duration, float speed, float magnitude);
    
    virtual void update(float time) override;
    virtual void stop() override;
    
CC_CONSTRUCTOR_ACCESS:
    SmoothShakeAction();
    virtual ~SmoothShakeAction() {}
    
    bool initWithDuration(float duration, float speed, float magnitude);
    
protected:
    float _speed;                       ///< frequency
    float _magnitude;                   ///< pixel
    float _randomStart;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(SmoothShakeAction);
};
