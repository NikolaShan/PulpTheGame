//
//  SegmentMaker.h
//  Pulp
//
//  Created by Mikhail Shulepov on 17/07/15.
//
//

#pragma once

#include <vector>

#include "math/Vec2.h"
#include "2d/CCNode.h"

namespace cocos2d {
    class Sprite;
    class GLProgram;
    class LayerColor;
}

class CircleMaker {
public:
    static cocos2d::LayerColor *makeCircleWithRadius(float innerRadius, float outerRadius);

};

class SegmentMaker {
public:
    SegmentMaker(): targetSprite(nullptr) {
        innerAngleOffset = 0.0f;// CC_DEGREES_TO_RADIANS(cocos2d::random(3.0f, 7.0f));
        if (cocos2d::random(0.0f, 1.0f) > 0.5f) {
            innerAngleOffset = -innerAngleOffset;
        }
    }
    ~SegmentMaker();
    
    CC_SYNTHESIZE(float, innerAngleOffset, InnerAngleOffset)
    
    void setAngle(const double angle) {
        this->angle = angle;
    }
    
    void setRadius(double near, double far);
    
    cocos2d::Sprite *makeSprite();
    
    //returns polygon points with coordinate system start from the circle center
    using ConvexPolygon = std::vector<cocos2d::Vec2>;
    std::vector<ConvexPolygon> makePhysicsPolygons() const;
    
private:
    cocos2d::Sprite *makeSpriteAsCircle();
    cocos2d::Sprite *makeSpriteAsSegment();
    
    ConvexPolygon makePhysicsPolygon(double startAngle, double endAngle) const;
    
    void prepareSprite();
    
private:
    double angle;
    double radiusNear;
    double radiusFar;
        
    cocos2d::Sprite *targetSprite;
};