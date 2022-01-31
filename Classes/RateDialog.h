//
//  RateDialog.h
//  Pulp
//
//  Created by Maxim Shmotin on 01/08/15.
//
//

#ifndef __Pulp__RateDialog__
#define __Pulp__RateDialog__

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"

NS_CC_BEGIN
class Label;
NS_CC_END

class RateDialog
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner {
public:
    static RateDialog* createNode();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref* target, const std::string &memberVariableName, cocos2d::Node* node) override;
    virtual void onEnter() override;
        
private:
    void openAppStorePage();
    void decline();
    void close();
    
private:
    friend class RateDialogLoader;
    CREATE_FUNC(RateDialog);
    RateDialog();
    virtual ~RateDialog();
    
    bool isHiding = false;
};

class RateDialogLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~RateDialogLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(RateDialogLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(RateDialog);
};


#endif /* defined(__Pulp__RateDialog__) */
