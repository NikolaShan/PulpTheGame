//
//  CollectionViewCCBItem.h
//  Pulp
//
//  Created by Mikhail Shulepov on 23/07/15.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"

#include "UICollectionViewItem.h"

namespace cocosbuilder {
    class CCBAnimationManager;
}

class CollectionViewCCBItem
: public cocos2d::CollectionViewItem
, public cocosbuilder::CCBMemberVariableAssigner {
public:
    static CollectionViewCCBItem *createFromCcb(const std::string &ccb, const cocos2d::Size &size);
    virtual ~CollectionViewCCBItem();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    
    cocos2d::Node *getNodeNamed(const std::string &name) const {
        return nodes.at(name);
    }
    
    virtual CollectionViewItem *createCloneInstance() const override;
    
    void runAnimation(const std::string &animation, float tweenDuration);
    
protected:
    virtual void updateSelectionState(bool selected, bool animated) override;
    
private:
    friend class CollectionViewCCBItemLoader;
    CREATE_FUNC(CollectionViewCCBItem);
    CollectionViewCCBItem();
    
private:
    cocosbuilder::CCBAnimationManager *animationManager;
    
    std::string ccbFile;
    cocos2d::Size size;
    
    cocos2d::Map<std::string, cocos2d::Node *> nodes;
};

class CollectionViewCCBItemLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~CollectionViewCCBItemLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CollectionViewCCBItemLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CollectionViewCCBItem);
};