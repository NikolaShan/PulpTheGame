//
//  RemoveAdsDialog.h
//  Pulp
//
//  Created by Mikhail Shulepov on 22/07/15.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

NS_CC_BEGIN
class Scene;
class Label;
NS_CC_END

class CCBButton;

class RemoveAdsDialog
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    static void show();
    virtual ~RemoveAdsDialog();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    void cancel();
    void buy();
    void restore();
    
private:
    std::string getBuyButtonTitle() const;
    
private:
    friend class RemoveAdsDialogLoader;
    CREATE_FUNC(RemoveAdsDialog);
    RemoveAdsDialog();
    
    bool isHiding = false;
};

class RemoveAdsDialogLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~RemoveAdsDialogLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(RemoveAdsDialogLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(RemoveAdsDialog);
};