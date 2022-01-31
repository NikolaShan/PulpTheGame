//
//  GuiUtils.h
//  Square
//
//  Created by Edward on 31.05.15.
//
//

#ifndef __Square__GuiUtils__
#define __Square__GuiUtils__

#include <string> 
#include "base/ccTypes.h"
#include "Localization.h"
#include "base/CCRefPtr.h"

#define SAFE_ASSIGN_AND_RETAIN(_TARGET_VAR_, _SOURCE_VAR_)\
do {\
    auto oldVar = _TARGET_VAR_;\
    _TARGET_VAR_ = _SOURCE_VAR_;\
    CC_SAFE_RETAIN(_TARGET_VAR_);\
    CC_SAFE_RELEASE(oldVar);\
} while(0)

#define ASSIGN_VARIABLE(_TARGET_NAME_,_VARIABLE_,_TYPE_)\
if (_TARGET_NAME_ == memberVariableName) {\
    _TYPE_ *var = dynamic_cast<_TYPE_ *>(node);\
    CC_ASSERT(var);\
    SAFE_ASSIGN_AND_RETAIN(_VARIABLE_, var);\
    return true;\
}

#define ASSIGN_SMART_VARIABLE(_TARGET_NAME_,_VARIABLE_,_TYPE_)\
if (_TARGET_NAME_ == memberVariableName) {\
    _TYPE_ *var = dynamic_cast<_TYPE_ *>(node);\
    CC_ASSERT(var);\
    _VARIABLE_ = var;\
    return true;\
}

#define CCB_LABEL_SET_LOCALIZED_STRING(_TARGET_NAME_, _STRING_KEY_)\
if (_TARGET_NAME_ == memberVariableName) {\
    Label *label = dynamic_cast<Label *>(node);\
    CC_ASSERT(label);\
    label->setString(Localization::getInstance()->getString(_STRING_KEY_));\
    return true;\
}


#define SETUP_LABEL(_TARGET_NAME_, _TITLE_)\
if (_TARGET_NAME_ == memberVariableName) {\
    Label *label = dynamic_cast<Label *>(node);\
    CC_ASSERT(label);\
    label->setString(_TITLE_);\
    return true;\
}

#define SETUP_LABEL_MAX_WIDTH(_TARGET_NAME_, _TITLE_, __MAX_WIDTH__)\
if (_TARGET_NAME_ == memberVariableName) {\
    Label *label = dynamic_cast<Label *>(node);\
    CC_ASSERT(label);\
    label->setString(_TITLE_);\
    auto width = label->getBoundingBox().size.width;\
    if (width > __MAX_WIDTH__) {\
        const float scale = label->getScale() * __MAX_WIDTH__ / width;\
        label->setScale(scale);\
    }\
    return true;\
}

#define SETUP_CUSTOM_BUTTON(_TARGET_NAME_, _TITLE_, _FUNC_NAME_)\
if (_TARGET_NAME_ == memberVariableName) {\
    Button *btn = dynamic_cast<Button *>(node);\
    CC_ASSERT(btn);\
    btn->setTitle(_TITLE_);\
    btn->setCallback([this](){_FUNC_NAME_();});\
    return true;\
}

#define SETUP_CUSTOM_IMAGE_BUTTON(_TARGET_NAME_, _IMAGE_, _FUNC_NAME_)\
if (_TARGET_NAME_ == memberVariableName) {\
    Button *btn = dynamic_cast<Button *>(node);\
    CC_ASSERT(btn);\
    btn->setIcon(_IMAGE_);\
    btn->setCallback([this](){_FUNC_NAME_();});\
    return true;\
}

#define SETUP_CUSTOM_CCBBUTTON(_TARGET_NAME_, _TITLE_, _FUNC_NAME_)\
if (_TARGET_NAME_ == memberVariableName) {\
    CCBButton *btn = dynamic_cast<CCBButton *>(node);\
    CC_ASSERT(btn);\
    btn->setTitle(_TITLE_);\
    btn->setCallback([this](){_FUNC_NAME_();});\
    return true;\
}

#define SETUP_CUSTOM_IMAGE_CCBBUTTON(_TARGET_NAME_, _IMAGE_, _FUNC_NAME_)\
if (_TARGET_NAME_ == memberVariableName) {\
    CCBButton *btn = dynamic_cast<CCBButton *>(node);\
    CC_ASSERT(btn);\
    btn->setIcon(_IMAGE_);\
    btn->setCallback([this](){_FUNC_NAME_();});\
    return true;\
}

#define SETUP_CUSTOM_IMAGE_TITLE_CCBBUTTON(_TARGET_NAME_, _IMAGE_, _TITLE_, _FUNC_NAME_)\
if (_TARGET_NAME_ == memberVariableName) {\
    CCBButton *btn = dynamic_cast<CCBButton *>(node);\
    CC_ASSERT(btn);\
    btn->setTitle(_TITLE_);\
    btn->setIcon(_IMAGE_);\
    btn->setCallback([this](){_FUNC_NAME_();});\
    return true;\
}

NS_CC_BEGIN
    class Sprite;
    class EventListenerTouchOneByOne;
    class EventListener;
NS_CC_END

namespace GuiUtils {
    enum class TouchState {
        DOWN,
        MOVE,
        UP
    };
    
    cocos2d::Color4B getColorARGBFromString(const std::string hexColor);
    cocos2d::Color4B getColorRGBAFromString(const std::string hexColor);
    
    cocos2d::EventListener *setupSimpleTouchHandler(cocos2d::Node *node, std::function<void(TouchState state, bool inside)> callback);
    
    //return false to stop
    void iterateRecursively(cocos2d::Node *parent, std::function<bool(cocos2d::Node *)>callback);
    void enableCascadeOpacityRecursively(cocos2d::Node *node, bool enable);
    void enableCascadeColorRecursively(cocos2d::Node *node, bool enable);
    
    void setGlobalZOrderRecursively(cocos2d::Node *node, int globalZOrder);
    
    void spriteSetImage(cocos2d::Sprite *sprite, const std::string &frameOrFile);
    
    cocos2d::EventListenerTouchOneByOne *setupSimpleTouchHandler(cocos2d::Node *node, float moveThreshold, std::function<void(TouchState state, bool inside)> callback);
    
    cocos2d::Rect getBoundingBoxInWorldSpace(cocos2d::Node *node);
    void setupEscapePressHandler(cocos2d::Node *node, std::function<void (void)> callback);
};

#endif /* defined(__Square__GuiUtils__) */
