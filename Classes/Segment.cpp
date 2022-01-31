//
//  Segment.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 29/06/16.
//
//

#include "Segment.h"
#include "Resources/Resources.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/CCGLProgram.h"

USING_NS_CC;

Segment::Segment(cocos2d::Node *pnode, float pangleSize): angleSize(pangleSize), node(pnode) {
    node->retain();
    node->setOpacityModifyRGB(false);
    addChild(node);
    
    setCascadeOpacityEnabled(true);
    node->setCascadeOpacityEnabled(true);
    
    GLProgram *glProgram = GLProgramCache::getInstance()->getGLProgram("ColorReplacer");
    if (!glProgram) {
        glProgram = GLProgram::createWithFilenames(R::Shaders::SHADER_COLOR_REPLACEMENT_NO_MVP_VERT, R::Shaders::SHADER_COLOR_REPLACEMENT_NO_MVP_FRAG);
        glProgram->link();
        
        glProgram->updateUniforms();
        
        GLProgramCache::getInstance()->addGLProgram(glProgram, "ColorReplacer");
    }
    
    program = GLProgramState::create(glProgram);
    node->setGLProgramState(program);
}

void Segment::setShadowColor(const cocos2d::Color3B &color) {
    shadowColor = color;
    program->setUniformVec4("u_replacementColor", Vec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0f));
}

SegmentShadowTintTo* SegmentShadowTintTo::create(float duration, GLubyte red, GLubyte green, GLubyte blue)
{
    SegmentShadowTintTo *tintTo = new (std::nothrow) SegmentShadowTintTo();
    if (tintTo && tintTo->initWithDuration(duration, red, green, blue))
    {
        tintTo->autorelease();
        return tintTo;
    }
    
    delete tintTo;
    return nullptr;
}

SegmentShadowTintTo* SegmentShadowTintTo::create(float duration, const Color3B& color)
{
    return create(duration, color.r, color.g, color.b);
}

bool SegmentShadowTintTo::initWithDuration(float duration, GLubyte red, GLubyte green, GLubyte blue)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _to = Color3B(red, green, blue);
        return true;
    }
    
    return false;
}

SegmentShadowTintTo* SegmentShadowTintTo::clone() const
{
    // no copy constructor
    return SegmentShadowTintTo::create(_duration, _to.r, _to.g, _to.b);
}

SegmentShadowTintTo* SegmentShadowTintTo::reverse() const
{
    CCASSERT(false, "reverse() not supported in TintTo");
    return nullptr;
}

void SegmentShadowTintTo::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
    _segmentTarget = dynamic_cast<Segment *>(target);
    if (_segmentTarget)
    {
        _from = _segmentTarget->getShadowColor();
    }
}

void SegmentShadowTintTo::update(float time)
{
    if (_target)
    {
        _segmentTarget->setShadowColor(Color3B(GLubyte(_from.r + (_to.r - _from.r) * time),
                                  (GLubyte)(_from.g + (_to.g - _from.g) * time),
                                  (GLubyte)(_from.b + (_to.b - _from.b) * time)));
    }
}
