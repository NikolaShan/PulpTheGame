//
//  GuiUtils.cpp
//  Square
//
//  Created by Edward on 31.05.15.
//
//

#include <algorithm>
#include <iostream>
#include <string>

#include "2d/CCSpriteFrameCache.h"
#include "2d/CCSprite.h"
#include "base/CCEventListener.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventListenerKeyboard.h"
#include "base/CCDirector.h"

#include "GuiUtils.h"
#include "StringUtils.h"
#include "Utils/StringUtils.h"

USING_NS_CC;

cocos2d::Color4B GuiUtils::getColorARGBFromString(const std::string hexColor) {
    if (std::find(hexColor.begin(), hexColor.end(), ',') != hexColor.end()) {
        const auto colorComponents = ::StringUtils::split(hexColor, ",");
        if (colorComponents.size() == 4) {
            const GLubyte a = ::StringUtils::toNumber<GLubyte>(colorComponents[0]);
            const GLubyte r = ::StringUtils::toNumber<GLubyte>(colorComponents[1]);
            const GLubyte g = ::StringUtils::toNumber<GLubyte>(colorComponents[2]);
            const GLubyte b = ::StringUtils::toNumber<GLubyte>(colorComponents[3]);
            return cocos2d::Color4B(r, g, b, a);
        } else if (colorComponents.size() == 3) {
            const GLubyte a = 255;
            const GLubyte r = ::StringUtils::toNumber<GLubyte>(colorComponents[0]);
            const GLubyte g = ::StringUtils::toNumber<GLubyte>(colorComponents[1]);
            const GLubyte b = ::StringUtils::toNumber<GLubyte>(colorComponents[2]);
            return cocos2d::Color4B(r, g, b, a);
        }
    }
    
    unsigned long colorValue = strtoul(hexColor.c_str(), nullptr, 16);
    if (hexColor.length() == 8) {
        const int a = (colorValue & 0xFF000000) >> 24;
        const int r = (colorValue & 0x00FF0000) >> 16;
        const int g = (colorValue & 0x0000FF00) >> 8;
        const int b = (colorValue & 0x000000FF);
        return cocos2d::Color4B(r, g, b, a);
    } else if (hexColor.length() == 6) {
        const int r = (colorValue & 0xFF0000) >> 16;
        const int g = (colorValue & 0x00FF00) >> 8;
        const int b = (colorValue & 0x0000FF);
        return Color4B(r, g, b, 255);
    }
    return Color4B::WHITE;
}

cocos2d::Color4B GuiUtils::getColorRGBAFromString(const std::string hexColor) {
    if (std::find(hexColor.begin(), hexColor.end(), ',') != hexColor.end()) {
        const auto colorComponents = ::StringUtils::split(hexColor, ",");
        if (colorComponents.size() >= 3) {
            const GLubyte r = ::StringUtils::toNumber<GLubyte>(colorComponents[0]);
            const GLubyte g = ::StringUtils::toNumber<GLubyte>(colorComponents[1]);
            const GLubyte b = ::StringUtils::toNumber<GLubyte>(colorComponents[2]);
            GLubyte a = 255;
            if (colorComponents.size() >= 4) {
                ::StringUtils::toNumber<GLubyte>(colorComponents[3]);
            }
            return cocos2d::Color4B(r, g, b, a);
        }
    }
    
    const unsigned long colorValue = strtoul(hexColor.c_str(), nullptr, 16);
    if (hexColor.length() == 8) {
        const int a = (colorValue & 0x000000FF);
        const int r = (colorValue & 0xFF000000) >> 24;
        const int g = (colorValue & 0x00FF0000) >> 16;
        const int b = (colorValue & 0x0000FF00) >> 8;
        return Color4B(r, g, b, a);
    } else if (hexColor.length() == 6) {
        const int r = (colorValue & 0xFF0000) >> 16;
        const int g = (colorValue & 0x00FF00) >> 8;
        const int b = (colorValue & 0x0000FF);
        return Color4B(r, g, b, 255);
    }
    return Color4B::WHITE;
}


EventListener *GuiUtils::setupSimpleTouchHandler(Node *node, std::function<void(TouchState, bool)> callback) {
    auto touchHandler = EventListenerTouchOneByOne::create();
    touchHandler->setSwallowTouches(true);
    touchHandler->onTouchBegan = [node, callback](Touch *touch, Event *event) -> bool {
        if (!node->isVisible()) {
            return false;
        }
        const auto touchLocation = node->getParent()->convertToNodeSpace(touch->getLocation());
        if (node->getBoundingBox().containsPoint(touchLocation)) {
            callback(TouchState::DOWN, true);
            return true;
        }
        return false;
    };
    touchHandler->onTouchMoved = [node, callback](Touch *touch, Event *event) {
        const auto touchLocation = node->getParent()->convertToNodeSpace(touch->getLocation());
        callback(TouchState::MOVE, node->getBoundingBox().containsPoint(touchLocation));
    };
    touchHandler->onTouchEnded = touchHandler->onTouchCancelled = [node, callback](Touch *touch, Event *event) {
        const auto touchLocation = node->getParent()->convertToNodeSpace(touch->getLocation());
        callback(TouchState::UP, node->getBoundingBox().containsPoint(touchLocation));
    };
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(node);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchHandler, node);
    return touchHandler;
}

void GuiUtils::spriteSetImage(cocos2d::Sprite *sprite, const std::string &frameOrFile) {
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameOrFile);
    if (frame) {
        sprite->setSpriteFrame(frame);
        return;
    }
    sprite->setTexture(frameOrFile);
}

namespace GuiUtils {
    bool iterateRecursivelyImpl(Node *parent, std::function<bool(Node *)>callback) {
        if (!callback(parent)) {
            return false;
        };
        auto &children = parent->getChildren();
        for (auto child : children) {
            if (!iterateRecursivelyImpl(child, callback)) {
                return false;
            }
        }
        return true;
    }
}


void GuiUtils::iterateRecursively(Node *parent, std::function<bool(Node *)>callback) {
    iterateRecursivelyImpl(parent, callback);
}

void GuiUtils::enableCascadeOpacityRecursively(cocos2d::Node *node, bool enable) {
    iterateRecursively(node, [enable](Node *entry) {
        entry->setCascadeOpacityEnabled(enable);
        return true;
    });
}

void GuiUtils::enableCascadeColorRecursively(cocos2d::Node *node, bool enable) {
    iterateRecursively(node, [enable](Node *entry) {
        entry->setCascadeColorEnabled(enable);
        return true;
    });
}

EventListenerTouchOneByOne *GuiUtils::setupSimpleTouchHandler(Node *node, float moveThreshold, std::function<void(TouchState, bool)> callback) {
    auto touchHandler = EventListenerTouchOneByOne::create();
    touchHandler->setSwallowTouches(true);
    touchHandler->onTouchBegan = [node, callback](Touch *touch, Event *event) -> bool {
        if (!node->isVisible()) {
            return false;
        }
        const auto touchLocation = node->getParent()->convertToNodeSpace(touch->getLocation());
        if (node->getBoundingBox().containsPoint(touchLocation)) {
            callback(TouchState::DOWN, true);
            return true;
        }
        return false;
    };
    touchHandler->onTouchMoved = [node, callback](Touch *touch, Event *event) {
        const auto touchLocation = node->getParent()->convertToNodeSpace(touch->getLocation());
        callback(TouchState::MOVE, node->getBoundingBox().containsPoint(touchLocation));
    };
    touchHandler->onTouchEnded = touchHandler->onTouchCancelled = [node, callback, moveThreshold](Touch *touch, Event *event) {
        const auto touchLocation = node->getParent()->convertToNodeSpace(touch->getLocation());
        const auto startLocation = node->getParent()->convertToNodeSpace(touch->getStartLocation());
        if (moveThreshold <= 0 || touchLocation.distance(startLocation) < moveThreshold) {
            callback(TouchState::UP, node->getBoundingBox().containsPoint(touchLocation));
        }
    };
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(node);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchHandler, node);
    return touchHandler;
}

cocos2d::Rect GuiUtils::getBoundingBoxInWorldSpace(cocos2d::Node *node) {
    auto sourceRect = node->getBoundingBox();
    auto min = node->getParent()->convertToWorldSpace(Vec2(sourceRect.getMinX(), sourceRect.getMinY()));
    auto max = node->getParent()->convertToWorldSpace(Vec2(sourceRect.getMaxX(), sourceRect.getMaxY()));
    sourceRect.origin = min;
    sourceRect.size = cocos2d::Size(max.x - min.x, max.y - min.y);
    return sourceRect;
}

void GuiUtils::setupEscapePressHandler(cocos2d::Node *node, std::function<void (void)> callback) {
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [](EventKeyboard::KeyCode code, Event *event) {
        if (code == EventKeyboard::KeyCode::KEY_ESCAPE) {
            event->stopPropagation();
        }
    };
    keyboardListener->onKeyReleased = [callback](EventKeyboard::KeyCode code, Event *event) {
        if (code == EventKeyboard::KeyCode::KEY_ESCAPE) {
            callback();
            event->stopPropagation();
        }
    };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, node);
}
