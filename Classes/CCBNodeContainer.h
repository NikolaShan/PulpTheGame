//
//  CCBNodeContainer.h
//  Pulp
//
//  Created by Mikhail Shulepov on 22/02/16.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

class CCBNodeContainer
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    virtual ~CCBNodeContainer();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    cocos2d::Map<std::string, cocos2d::Node *> getInnerNodes() const {
        return nodes;
    }
    
    cocos2d::Node *getInnerNodeNamed(const std::string &name) {
        return nodes.at(name);
    }
    
private:
    friend class CCBNodeContainerLoader;
    CREATE_FUNC(CCBNodeContainer);
    CCBNodeContainer();
    
    cocos2d::Map<std::string, cocos2d::Node *> nodes;
};

class CCBNodeContainerLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~CCBNodeContainerLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCBNodeContainerLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCBNodeContainer);
};