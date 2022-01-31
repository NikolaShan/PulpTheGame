//
//  CCBAnimationRunner.cpp
//  Square
//
//  Created by Mikhail Shulepov on 07/06/15.
//
//

#include "CCBAnimationRunner.h"

USING_NS_CC;
using namespace cocosbuilder;

CCBAnimationRunner *CCBAnimationRunner::run(cocos2d::Node *target, const std::string &animationName, Completion completion) {
    auto ret = new CCBAnimationRunner(target, animationName, completion);
    ret->autorelease();
    return ret;
}

CCBAnimationRunner::CCBAnimationRunner(cocos2d::Node *target, const std::string &animationName, Completion pCompletion) {
    CCBAnimationManager *animator = dynamic_cast<CCBAnimationManager *>(target->getUserObject());
    if (animator) {
        if (animator->getSequenceId(animationName.c_str()) >= 0) {
            if (pCompletion) {
                completion = pCompletion;
                animator->setDelegate(this);
            }
            animator->runAnimationsForSequenceNamed(animationName.c_str());
        } else {
            if (pCompletion) {
                pCompletion();
            }
            log("Warning: CCBAnimationRunner - animation named %s not found", animationName.c_str());
        }
    } else if (pCompletion) {
        pCompletion();
    }
}

void CCBAnimationRunner::completedAnimationSequenceNamed(cocosbuilder::CCBAnimationManager *manager, const char *name) {
    if (completion) {
        completion();
    }
    manager->setDelegate(nullptr);
}

bool CCBAnimationRunner::hasAnimation(cocos2d::Node *target, const std::string &animationName) {
    CCBAnimationManager *animator = dynamic_cast<CCBAnimationManager *>(target->getUserObject());
    if (animator) {
        if (animator->getSequenceId(animationName.c_str()) >= 0) {
            return true;
        }
    }
    return false;
}