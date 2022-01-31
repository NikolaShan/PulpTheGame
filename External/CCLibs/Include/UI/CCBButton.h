//
//  CCBButton.h
//  Square
//
//  Created by Mikhail Shulepov on 12/07/15.
//
//

#ifndef __Square__CCBButton__
#define __Square__CCBButton__

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

NS_CC_BEGIN
class Sprite;
class Label;
NS_CC_END

class CCBButton
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    using Callback = std::function<void(void)>;
    
    virtual ~CCBButton();
    
    bool isEnabled() const {
        return enabled;
    }
    virtual void setEnabled(bool enabled);
    
    void setCallback(Callback callback) { this->callback = callback; }
    Callback getCallback() const { return callback; }
    
    void setTitle(const std::string &title);
    void setIcon(const std::string &iconPath);
    
    cocos2d::Sprite *getIconView() const {
        return iconView;
    }
    
    cocos2d::Label *getTitleLabel() const {
        return titleLabel;
    }
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref* target, const std::string &memberVariableName, cocos2d::Node* node) override;
    virtual void onNodeLoaded(cocos2d::Node * pNode, cocosbuilder::NodeLoader * pNodeLoader) override;
    virtual bool onAssignCCBCustomProperty(cocos2d::Ref* target, const std::string &memberVariableName, const cocos2d::Value& value) override;
    
    static void setDefaultSound(const std::string &sound) {
        DEFAULT_SOUND = sound;
    }
    
    void setSound(const std::string &sound) {
        this->sound = sound;
    }
    
    virtual void setUserObject(cocos2d::Ref *object) override;
    
protected:
    CCBButton();
    void animateToState(const std::string &stateName, float tweenDuration);

private:
    friend class CCBButtonLoader;
    CREATE_FUNC(CCBButton);

    void setupTouchHandler();
    bool isVisibleRecursively() const;
    
private:
    static const float DEFAULT_TWEEN_DURATION;
    static const std::string STATE_NORMAL_NAME;
    static const std::string STATE_PRESSED_NAME;
    static const std::string STATE_DISABLED_NAME;
    
    static std::string DEFAULT_SOUND;
    
    Callback callback;
    bool enabled = true;
    
    cocos2d::Sprite *iconView;
    cocos2d::Label *titleLabel;
    cocosbuilder::CCBAnimationManager *animator;
    std::string sound;
};

class CCBButtonLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~CCBButtonLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCBButtonLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCBButton);
};


#endif /* defined(__Square__CCBButton__) */
