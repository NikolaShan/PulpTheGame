//
//  ExitConfirmationDialog.h
//  Pulp
//
//  Created by Mikhail Shulepov on 15.12.15.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

class ExitConfirmationDialog
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    static ExitConfirmationDialog *createFromCcb();
    virtual ~ExitConfirmationDialog();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    void exitGame();
    void dismiss();
    
private:
    friend class ExitConfirmationDialogLoader;
    CREATE_FUNC(ExitConfirmationDialog);
    ExitConfirmationDialog();
    
    bool isHiding = false;
};

class ExitConfirmationDialogLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~ExitConfirmationDialogLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(ExitConfirmationDialogLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(ExitConfirmationDialog);
};