//
//  SettingsDialog.h
//  Pulp
//
//  Created by Mikhail Shulepov on 03/08/16.
//
//

#pragma once

#include "GameDialog.h"
#include "base/CCDirector.h"

class CCBToggle;

class SettingsDialog: public GameDialog {
public:
    CCB_DIALOG_SHOW_SYNTHESIZE(SettingsDialog);
    
    virtual ~SettingsDialog();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    void toggleColorblindMode(CCBToggle *btn);
    void toggleMusic(CCBToggle *btn);
    void toggleSounds(CCBToggle *btn);
    void toggleNightMode(CCBToggle *btn);
    
private:
    friend class SettingsDialogLoader;
    CREATE_FUNC(SettingsDialog);
    SettingsDialog();
    
    cocos2d::Map<std::string, CCBToggle *> options;
};

class SettingsDialogLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~SettingsDialogLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(SettingsDialogLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(SettingsDialog);
};
