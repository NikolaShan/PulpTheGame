//
//  UIDialog.h
//  AAHedgehog
//
//  Created by Mikhail Shulepov on 16/01/16.
//
//

#pragma once

#include "base/CCRefPtr.h"
#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

#define CCB_DIALOG_SHOW_SYNTHESIZE(__TYPE__) \
static __TYPE__ *show() {\
    auto scene = cocos2d::Director::getInstance()->getRunningScene();\
    return showInside(scene);\
}\
\
static __TYPE__ *showInside(cocos2d::Node *container) {\
    __TYPE__ *rootNode = createFromCcb(container->getContentSize());\
    rootNode->willShowInside(container);\
    container->addChild(rootNode);\
    return rootNode;\
}\
\
static __TYPE__ *createFromCcb(const cocos2d::Size &size)


class CCBDialog
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    virtual ~CCBDialog();
    
    virtual bool onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node * pNode, cocosbuilder::NodeLoader * pNodeLoader) override;
    
    CC_SYNTHESIZE(bool, closeOnTouchOutside, CloseOnTouchOutside)
    CC_SYNTHESIZE(bool, escapeClosable, EscapeClosable)
    
    virtual void dismiss();
   
private:
    void handleTouch(cocos2d::Touch *touch);
    
protected:
    CCBDialog();
    virtual void willShowInside(cocos2d::Node *node) {}
    
    cocos2d::Size designScreenSizeInDpi {0, 0};
    cocos2d::RefPtr<cocos2d::Node> contentNode;
    
    bool isHiding = false;
};

