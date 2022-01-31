//
// Created by Mike on 09.02.14.
//

#include <map>
#include "base/CCVector.h"

#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCScheduler.h"

#include "Utils/StringUtils.h"

#include "EventDispatcherHelper.h"

USING_NS_CC;

class EventDispatcherHelper::EventListenerHolder : public cocos2d::Ref {
public:
    static EventListenerHolder *create(cocos2d::EventListenerCustom *eventListener, const size_t id) {
        auto ret = new EventListenerHolder(eventListener, id);
        ret->autorelease();
        return ret;
    }
    EventListenerHolder(cocos2d::EventListenerCustom *eventListener, const size_t id);
    virtual ~EventListenerHolder();
    
    void removeEventListener();
    size_t getTargetID() const { return _targetId; }
    cocos2d::EventListenerCustom * getEventListener() const { return _eventListener; }
    
private:
    const size_t _targetId;
    cocos2d::EventListenerCustom * const _eventListener;
};


class EventDispatcherHelper::Implementation {
public:
    std::map< std::string, cocos2d::Vector<EventListenerHolder *> > listeners;
};

EventDispatcherHelper &EventDispatcherHelper::getInstance() {
    static EventDispatcherHelper instance;
    return instance;
}

EventDispatcherHelper::EventDispatcherHelper() {
    pimpl = ImplPtr(new Implementation);
}

EventDispatcherHelper::~EventDispatcherHelper() {

}

void EventDispatcherHelper::dispatchEvent(const std::string &name, cocos2d::Value value) {
    if (Director::getInstance()->getEventDispatcher()->isEnabled()) {
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(name, &value);
    } else {
        Director::getInstance()->getScheduler()->schedule([this, name, value](float dt) {
            dispatchEvent(name, value);
        }, this, 0.0f, 0, 0.05f, false, name + ::StringUtils::toString(rand()));
    }
}

void EventDispatcherHelper::dispatchEvent(const std::string &name, void *userData) {
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(name, userData);
}

void EventDispatcherHelper::addEventListener(const std::string &eventName, std::function<void(EventCustom*)> callback, void *targetPtr) {
    EventListenerCustom *listener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(eventName, callback);
    if (pimpl->listeners.find(eventName) == pimpl->listeners.end()) {
        pimpl->listeners.insert(std::make_pair(eventName, cocos2d::Vector<EventListenerHolder *>()));
    }
    auto &eventListeners = pimpl->listeners[eventName];
    eventListeners.pushBack(EventListenerHolder::create(listener, (size_t)targetPtr));
}

void EventDispatcherHelper::removeEventListener(const std::string &eventName, void *targetPtr) {
    if (pimpl->listeners.find(eventName) != pimpl->listeners.end()) {
        const size_t id = (size_t)targetPtr;
        auto &eventListeners = pimpl->listeners[eventName];
        cocos2d::Vector<EventListenerHolder *> toErase;
        for (EventListenerHolder *holder : eventListeners) {
            if (id == 0 || holder->getTargetID() == id) {
                toErase.pushBack(holder);
            }
        }
        for (EventListenerHolder *eraseItem : toErase) {
            eventListeners.eraseObject(eraseItem);
        }
    }
}

void EventDispatcherHelper::removeEventListenersForTarget(void *targetPtr) {
    const size_t id = (size_t)targetPtr;
    cocos2d::Vector<EventListenerHolder *> toErase;
    for (auto &entry : pimpl->listeners) {
        auto &eventHolders = entry.second;
        for (EventListenerHolder *holder : eventHolders) {
            if (holder->getTargetID() == id) {
                toErase.pushBack(holder);
            }
        }

        for (EventListenerHolder *eraseItem : toErase) {
            eventHolders.eraseObject(eraseItem);
        }
        toErase.clear();
    }
}


#pragma mark -
#pragma mark Event Listener Holder

EventDispatcherHelper::EventListenerHolder::EventListenerHolder(cocos2d::EventListenerCustom *eventListener, const size_t id)
: _eventListener(eventListener)
, _targetId(id) {
    _eventListener->retain();
}

EventDispatcherHelper::EventListenerHolder::~EventListenerHolder() {
    //reference count: first - our, second - cocos2d::Dispatcher
    if (_eventListener->getReferenceCount() <= 2) {
        removeEventListener();
    }
    _eventListener->release();
}

void EventDispatcherHelper::EventListenerHolder::removeEventListener() {
    Director::getInstance()->getEventDispatcher()->removeEventListener(_eventListener);
}
