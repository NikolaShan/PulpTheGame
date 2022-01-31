//
//  CCBDialog.cpp
//  AAHedgehog
//
//  Created by Mikhail Shulepov on 16/01/16.
//
//

#include "CCBDialog.h"

#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"

#include "Utils/GuiUtils.h"
#include "Utils/AppUtils.h"

#include "CCBAnimationRunner.h"

USING_NS_CC;
using namespace cocosbuilder;


CCBDialog::CCBDialog() {
    GuiUtils::setupSimpleTouchHandler(this, 30, [this](GuiUtils::TouchState state, bool inside) {
        if (state == GuiUtils::TouchState::UP && this->closeOnTouchOutside) {
            this->dismiss();
        }
    });
    GuiUtils::setupEscapePressHandler(this, [this]() {
        if (this->escapeClosable) {
            this->dismiss();
        }
    });
}

CCBDialog::~CCBDialog() {
    
}

bool CCBDialog::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    if (memberVariableName == "ContentNode") {
        contentNode = node;
        const Size sizeInDpi = AppUtils::getScreenSizeInDpi();
        if (designScreenSizeInDpi.width > 0.1f) {
            if (sizeInDpi.width > designScreenSizeInDpi.width) {
                const float scale = designScreenSizeInDpi.width / sizeInDpi.width;
                node->setScale(scale);
            }
        } else if (designScreenSizeInDpi.height > 0.1f) {
            if (sizeInDpi.height > designScreenSizeInDpi.height) {
                const float scale = designScreenSizeInDpi.height / sizeInDpi.height;
                node->setScale(scale);
            }
        }
        
        return true;
    }
    
    return false;
}

void CCBDialog::onNodeLoaded(cocos2d::Node * pNode, cocosbuilder::NodeLoader * pNodeLoader) {
    if (contentNode) {
        GuiUtils::setupSimpleTouchHandler(contentNode, [](GuiUtils::TouchState, bool) {
            //just swallow touches
        });
    }
}

void CCBDialog::dismiss() {
    if (isHiding) {
        return;
    }
    CCBAnimationRunner::run(this, "Dismiss", [this]() {
        removeFromParent();
        this->isHiding = false;
    });
    isHiding = true;
}