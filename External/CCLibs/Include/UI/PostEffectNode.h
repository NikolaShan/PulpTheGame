//
//  PostEffectNode.hpp
//  WordsGame
//
//  Created by Mikhail Shulepov on 02/09/16.
//
//

#pragma once

#include "2d/CCNode.h"

class PostEffectNode: public cocos2d::Node {
public:
    static PostEffectNode *create() {
        auto ret = new PostEffectNode();
        ret->autorelease();
        return ret;
    }
    
    static PostEffectNode *create(const cocos2d::Vector<cocos2d::GLProgramState *> &passes) {
        auto ret = create();
        ret->setShaderPasses(passes);
        return ret;
    }
    
    virtual ~PostEffectNode();
    
    CC_SYNTHESIZE(bool, postEffectsEnabled, PostEffectsEnabled)
    CC_PROPERTY(bool, staticDrawing, StaticDrawing)
    CC_PROPERTY(float, quality, Quality)
    
    void requestRedraw() {
        shouldRedraw = true;
    }
    
    void setShaderPasses(const cocos2d::Vector<cocos2d::GLProgramState *> &passes) {
        this->passes = passes;
        dirty = true;
    }
    
    cocos2d::GLProgramState *getShaderPass(int pass) const {
        return passes.at(pass);
    }
    
    size_t getShaderPassesCount() const {
        return passes.size();
    }
    
    virtual void setContentSize(const cocos2d::Size &size) override {
        Node::setContentSize(size);
        dirty = true;
        sizeDirty = true;
    }
    
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
    virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags) override;
    
private:
    void checkRenderTextures();
    void recreateRtSprites();
    void redraw(cocos2d::Renderer *renderer);

private:
    cocos2d::Vector<cocos2d::GLProgramState *> passes;
    
    cocos2d::Mat4 matrix;
    bool dirty = false;
    bool sizeDirty = false;
    cocos2d::Vector<cocos2d::RenderTexture *> renderTextures;
    cocos2d::Vector<cocos2d::Sprite *> rtSprites;
    bool shouldRedraw = true;
};