//
//  CCBButton.cpp
//  Square
//
//  Created by Mikhail Shulepov on 12/07/15.
//
//

#include "2d/CCSprite.h"
#include "2d/CCLabel.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "Utils/PLAudioEngine.h"

#include "Utils/GuiUtils.h"

#include "CCBButton.h"

USING_NS_CC;

std::string CCBButton::DEFAULT_SOUND = "";

const float CCBButton::DEFAULT_TWEEN_DURATION = 0.05f;
const std::string CCBButton::STATE_NORMAL_NAME = "Normal";
const std::string CCBButton::STATE_PRESSED_NAME = "Pressed";
const std::string CCBButton::STATE_DISABLED_NAME = "Disabled";

CCBButton::CCBButton(): titleLabel(nullptr), iconView(nullptr), animator(nullptr) {
    setupTouchHandler();
    sound = DEFAULT_SOUND;
}

CCBButton::~CCBButton() {
    CC_SAFE_RELEASE(titleLabel);
    CC_SAFE_RELEASE(iconView);
    CC_SAFE_RELEASE(animator);
}

void CCBButton::setTitle(const std::string &title) {
    if (titleLabel) {
        titleLabel->setString(title);
        if (titleLabel->getMaxLineWidth() > 1.0f) {
            const float scale = std::min(1.0f, titleLabel->getMaxLineWidth() / titleLabel->getContentSize().width);
            titleLabel->setScale(scale);
        }
    }
}

void CCBButton::setIcon(const std::string &iconPath) {
    if (iconView) {
        iconView->setVisible(true);
        GuiUtils::spriteSetImage(iconView, iconPath);
    }
}

void CCBButton::setEnabled(bool penabled) {
    if (enabled != penabled) {
        enabled = penabled;
        if (enabled) {
            animateToState(STATE_NORMAL_NAME, DEFAULT_TWEEN_DURATION);
        } else {
            animateToState(STATE_DISABLED_NAME, DEFAULT_TWEEN_DURATION);
        }
    }
}

bool CCBButton::isVisibleRecursively() const {
    if (!this->isVisible() || this->getDisplayedOpacity() < 1) {
        return false;
    }
    auto node = this->getParent();
    while (node) {
        if (!node->isVisible()) {
            return false;
        }
        node = node->getParent();
    }
    return true;
}

void CCBButton::setUserObject(cocos2d::Ref *object) {
    Node::setUserObject(object);
    if (!enabled) {
        animateToState(STATE_DISABLED_NAME, 0);
    }
}

bool CCBButton::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node)  {
    ASSIGN_VARIABLE("Title", titleLabel, Label);
    ASSIGN_VARIABLE("Icon", iconView, Sprite);
    return false;
}

bool CCBButton::onAssignCCBCustomProperty(Ref *target, const std::string &memberVariableName, const Value &value) {
    if (memberVariableName == "sound") {
        sound = value.asString();
        return true;
    }
    return false;
}

void CCBButton::onNodeLoaded(Node *pNode, cocosbuilder::NodeLoader *pNodeLoader) {
    if (pNode != this) {
        return;
    }
    if (titleLabel) {
        Size dimensions = titleLabel->getDimensions();
        if (dimensions.width > 0.1f) {
            titleLabel->setDimensions(0.0f, 0.0f);
            titleLabel->setMaxLineWidth(dimensions.width);//dimensions.width);
        }
    }
}

void CCBButton::animateToState(const std::string &stateName, float tweenDuration) {
    if (!animator) {
        animator = dynamic_cast<cocosbuilder::CCBAnimationManager *>(getUserObject());
        CC_SAFE_RETAIN(animator);
    }
    if (animator) {
        const int sequenceId = animator->getSequenceId(stateName.c_str());
        if (sequenceId >= 0) {
            animator->runAnimationsForSequenceIdTweenDuration(sequenceId, tweenDuration);
        }
    }
}

void CCBButton::setupTouchHandler() {
    auto touchHandler = EventListenerTouchOneByOne::create();
    touchHandler->setSwallowTouches(true);
    
    touchHandler->onTouchBegan = [this](Touch *touch, Event *event) -> bool {
        if (!isEnabled() || !isVisibleRecursively()) {
            return false;
        }
        const Vec2 touchLocation = getParent()->convertToNodeSpace(touch->getLocation());
        if (getBoundingBox().containsPoint(touchLocation)) {
            animateToState(STATE_PRESSED_NAME, DEFAULT_TWEEN_DURATION);
            return true;
        }
        return false;
    };
    
    touchHandler->onTouchMoved = [this](Touch *touch, Event *event) {
        const auto touchLocation = getParent()->convertToNodeSpace(touch->getLocation());
        const auto prevTouchLocation = getParent()->convertToNodeSpace(touch->getPreviousLocation());
        const Rect boundingBox = getBoundingBox();
        const bool currentInside = boundingBox.containsPoint(touchLocation);
        const bool prevInside = boundingBox.containsPoint(prevTouchLocation);
        if (!prevInside && currentInside) {
            animateToState(STATE_PRESSED_NAME, DEFAULT_TWEEN_DURATION);
        } else if (prevInside && !currentInside) {
            animateToState(STATE_NORMAL_NAME, DEFAULT_TWEEN_DURATION);
        }
    };
    
    touchHandler->onTouchEnded = touchHandler->onTouchCancelled = [this](Touch *touch, Event *event) {
        animateToState(STATE_NORMAL_NAME, DEFAULT_TWEEN_DURATION);
        const auto touchLocation = getParent()->convertToNodeSpace(touch->getLocation());
        if (callback && getBoundingBox().containsPoint(touchLocation)) {
            callback();
            if (!sound.empty()) {
                PLAudioEngine::getInstance()->playEffect(sound);
            }
        }
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchHandler, this);
}
