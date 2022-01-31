//
//  Pseudo3DNode.h
//  Pulp
//
//  Created by Mikhail Shulepov on 24/07/15.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

class Pseudo3DNode
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    static Pseudo3DNode *create(cocos2d::Node *mainNode, cocos2d::Node *shadowNode, float offset = 2.0f);
    
    virtual ~Pseudo3DNode();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual bool onAssignCCBCustomProperty(cocos2d::Ref* target, const std::string &memberVariableName, const cocos2d::Value& value) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    virtual void setRotation(float rotation) override;
    
    virtual void onExit() override;
    
    void extractShadow();
    
    virtual void setColor(const cocos2d::Color3B &color) override;
    virtual void setOpacity(GLubyte opacity) override;
    
    virtual void removeFromParentAndCleanup(bool cleanup) override;
    
private:
    friend class Pseudo3DNodeLoader;
    CREATE_FUNC(Pseudo3DNode);
    Pseudo3DNode();
    
    void updateShadowColor();
    void updateShadowOpacity();
    void updateShadowPosition();
    
private:
    bool extracted = false;
    float offset = 2.0f;
    cocos2d::Node *shadowNode;
    
    cocos2d::Color3B originalShadowColor;
};

class Pseudo3DNodeLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~Pseudo3DNodeLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(Pseudo3DNodeLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(Pseudo3DNode);
};