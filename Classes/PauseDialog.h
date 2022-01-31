//
//  PauseDialog.h
//  Pulp
//
//  Created by Mikhail Shulepov on 18.11.15.
//
//

#pragma once

#include "2d/CCNode.h"
#include "base/CCValue.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

class PauseDialog
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    static PauseDialog *createFromCcb();
    virtual ~PauseDialog();
    
    using Completion = std::function<void(bool resume)>;
    
    CC_SYNTHESIZE(Completion, completion, Completion);
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
       
private:
    void exitToMenu();
    void resumeGame();
    
private:
    friend class PauseDialogLoader;
    CREATE_FUNC(PauseDialog);
    PauseDialog();
    
    bool isHiding = false;
};

class PauseDialogLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~PauseDialogLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(PauseDialogLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(PauseDialog);
};