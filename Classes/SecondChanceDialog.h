//
//  SecondChanceDialog.h
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

class SecondChanceDialog
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    static SecondChanceDialog *createFromCcb();
    virtual ~SecondChanceDialog();
    
    using Completion = std::function<void(bool accepted)>;
    
    CC_SYNTHESIZE(Completion, completion, Completion);
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    void dismiss();
    
private:
    void accept();
    void decline();
    void alignTitle();
    
private:
    friend class SecondChanceDialogLoader;
    CREATE_FUNC(SecondChanceDialog);
    SecondChanceDialog();
    
private:
    cocos2d::Node *titleNode;
    bool isHiding = false;
};

class SecondChanceDialogLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~SecondChanceDialogLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(SecondChanceDialogLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(SecondChanceDialog);
};