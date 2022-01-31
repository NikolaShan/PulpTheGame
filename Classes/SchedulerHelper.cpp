//
// Created by Mike on 28.03.14.
//

#include <iostream>

#include "base/CCDirector.h"
#include "base/CCScheduler.h"
#include "Utils/StringUtils.h"

#include "SchedulerHelper.h"

USING_NS_CC;

class SchedulerHelper::RandomTarget : public cocos2d::Ref {
public:
    RandomTarget() {}
    virtual ~RandomTarget() {}
};

SchedulerHelper &SchedulerHelper::getInstance() {
    static SchedulerHelper instance;
    return instance;
}

void SchedulerHelper::schedule(std::function<void (void)> func, float delay) {
    auto targetRandom = new RandomTarget();
    auto scheduleFunc = [func, targetRandom](float dt){
        func();
        targetRandom->autorelease();
    };
    auto scheduler = Director::getInstance()->getScheduler();
    scheduler->schedule(
                        scheduleFunc,
                        targetRandom,
                        0,
                        0,
                        delay,
                        false,
                        ::StringUtils::toString(rand())
                        );
}
