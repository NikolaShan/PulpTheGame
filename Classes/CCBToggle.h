//
//  CCBToggle.h
//  Pulp
//
//  Created by Mikhail Shulepov on 04/08/16.
//
//

#pragma once

#include "UI/CCBButton.h"

class CCBToggle: public CCBButton {
public:
    virtual ~CCBToggle();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    void setSelected(bool selected, bool animated);
    
private:
    friend class CCBToggleLoader;
    CREATE_FUNC(CCBToggle);
    CCBToggle();
};

class CCBToggleLoader : public CCBButtonLoader {
public:
    virtual ~CCBToggleLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CCBToggleLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CCBToggle);
};