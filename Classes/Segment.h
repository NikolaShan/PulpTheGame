//
//  Segment.h
//  Pulp
//
//  Created by Mikhail Shulepov on 24/05/16.
//
//

#pragma once 

#include "2d/CCNode.h"
#include "2d/CCActionInterval.h"

struct cpBody;

class Segment: public cocos2d::Node {
public:
    static Segment *create(cocos2d::Node *pnode, float pangleSize) {
        auto ret = new Segment(pnode, pangleSize);
        ret->autorelease();
        return ret;
    }
    
    Segment(cocos2d::Node *pnode, float pangleSize);
    
    virtual ~Segment() {
        node->release();
    }
    
    CC_SYNTHESIZE(cocos2d::Color3B, segmentColor, SegmentColor);
    
    float getAngleSize() const { return angleSize; }
        
    virtual void setColor(const cocos2d::Color3B &color) override {
        cocos2d::Node::setColor(color);
        node->setColor(color);
    }
    
    void setShadowColor(const cocos2d::Color3B &color);
    const cocos2d::Color3B &getShadowColor() const {
        return shadowColor;
    }
    
    bool isActive() const {
        return node->getParent();
    }
    
    cocos2d::Node *getInnerNode() {
        return node;
    }
    
private:
    float angleSize;
    cocos2d::Node *node;
    cocos2d::GLProgramState *program;
    cocos2d::Color3B shadowColor;
};

class CC_DLL SegmentShadowTintTo : public cocos2d::ActionInterval
{
public:
    static SegmentShadowTintTo* create(float duration, GLubyte red, GLubyte green, GLubyte blue);
    static SegmentShadowTintTo* create(float duration, const cocos2d::Color3B& color);
    
    //
    // Overrides
    //
    virtual SegmentShadowTintTo* clone() const override;
    virtual SegmentShadowTintTo* reverse(void) const override;
    virtual void startWithTarget(cocos2d::Node *target) override;
    /**
     * @param time In seconds.
     */
    virtual void update(float time) override;
    
CC_CONSTRUCTOR_ACCESS:
    SegmentShadowTintTo() {}
    virtual ~SegmentShadowTintTo() {}    
    bool initWithDuration(float duration, GLubyte red, GLubyte green, GLubyte blue);
    
protected:
    cocos2d::Color3B _to;
    cocos2d::Color3B _from;
    Segment *_segmentTarget = nullptr;
    
private:
    CC_DISALLOW_COPY_AND_ASSIGN(SegmentShadowTintTo);
};