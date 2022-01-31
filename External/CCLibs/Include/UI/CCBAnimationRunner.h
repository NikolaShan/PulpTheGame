//
//  CCBAnimationRunner.h
//  Square
//
//  Created by Mikhail Shulepov on 07/06/15.
//
//

#ifndef __Square__CCBAnimationRunner__
#define __Square__CCBAnimationRunner__

#include "cocosbuilder/CCBAnimationManager.h"

class CCBAnimationRunner: public cocos2d::Ref, public cocosbuilder::CCBAnimationManagerDelegate {
public:
    using Completion = std::function<void()>;
    static CCBAnimationRunner *run(cocos2d::Node *target, const std::string &animationName, Completion completion);
    
    virtual void completedAnimationSequenceNamed(cocosbuilder::CCBAnimationManager *manager, const char *name) override;
    
    static bool hasAnimation(cocos2d::Node *target, const std::string &animationName);
    
private:
    CCBAnimationRunner(cocos2d::Node *target, const std::string &animationName, Completion completion);
    
private:
    Completion completion;
};

#endif /* defined(__Square__CCBAnimationRunner__) */
