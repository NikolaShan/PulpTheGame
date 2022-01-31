//
//  BulletNode.h
//  Pulp
//
//  Created by Mikhail Shulepov on 09/06/16.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

class BulletNode
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    static BulletNode *createFromCcb();
    virtual ~BulletNode();
    
    CC_SYNTHESIZE(cocos2d::Color3B, bulletColor, BulletColor)
    CC_SYNTHESIZE(cocos2d::Color3B, colorblindColor, ColorblindColor)
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    virtual void update(float dt) override;
        
private:
    friend class BulletNodeLoader;
    CREATE_FUNC(BulletNode);
    BulletNode();
    
    cocos2d::Sprite *nail = nullptr;
    cocos2d::Point prevPos;
    
    int updatesCount = 0;
};

class BulletNodeLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~BulletNodeLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(BulletNodeLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(BulletNode);
};