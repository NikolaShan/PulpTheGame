//
// Created by Mike on 09.02.14.
//



#ifndef __EventDispatcherHelper_H_
#define __EventDispatcherHelper_H_

#include "base/CCValue.h"
#include <memory>

NS_CC_BEGIN
    class EventCustom;
    class EventListenerCustom;
NS_CC_END

#define DISPATCH_EVENT(_NAME_,_USER_DATA_) EventDispatcherHelper::getInstance().dispatchEvent(_NAME_,_USER_DATA_)

class EventDispatcherHelper {
public:
    static EventDispatcherHelper &getInstance();
    ~EventDispatcherHelper();

    void dispatchEvent(const std::string &name, cocos2d::Value value);
    void dispatchEvent(const std::string &name, void *userData = nullptr);
    void addEventListener(const std::string &eventName, std::function<void(cocos2d::EventCustom *)> callback, void *targetPtr);
    void removeEventListener(const std::string &eventName, void *targetPtr);
    void removeEventListenersForTarget(void *targetPtr);

private:
    DISALLOW_COPY_AND_ASSIGN(EventDispatcherHelper);
    EventDispatcherHelper();
    class EventListenerHolder;
    
private:
    class Implementation;
    using ImplPtr = std::auto_ptr<Implementation>;
    ImplPtr pimpl;
};


#endif //__EventDispatcherHelper_H_
