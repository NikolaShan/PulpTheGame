//
//  AppTurboDialog.h
//  Pulp
//
//  Created by Mikhail Shulepov on 06/03/16.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

class AppTurboDialog
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    static AppTurboDialog *createFromCcb();
    virtual ~AppTurboDialog();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    void dismiss();
    
private:
    friend class AppTurboDialogLoader;
    CREATE_FUNC(AppTurboDialog);
    AppTurboDialog();
    
    bool isHiding = false;
};

class AppTurboDialogLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~AppTurboDialogLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(AppTurboDialogLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(AppTurboDialog);
};