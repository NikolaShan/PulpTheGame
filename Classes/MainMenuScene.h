//
//  MainMenuScene.h
//  Pulp
//
//  Created by Mikhail Shulepov on 17/07/15.
//
//

#pragma once

#include "base/CCRefPtr.h"
#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

NS_CC_BEGIN
class Scene;
class Label;
NS_CC_END

class CCBButton;
class CCBNodeContainer;

enum class MenuStyle {
    MAIN_MENU,
    DEFEAT,
    VICTORY,
    ENDLESS_DEFEAT
};

class MainMenuScene
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    static const int MAIN_MENU_TAG = 1101;
    
    static cocos2d::Scene *createWithCurrentStyle();
    static cocos2d::Scene *createScene(MenuStyle style);
    virtual ~MainMenuScene();
      
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    virtual void onEnter() override;
    
    void play();
    void playEndless();
    
    void showSkipOptions();
    void showSkipForAds();
    void chooseLevel();
    
    void askDisableAds();
    void showLeaderboards();
    void showAchievements();
    void share(CCBButton *button);
    void rate();
    void openMainMenu();
    void shareFacebook();
    void shareTwitter();
    void showSettings();
    
    void toggleGameServices();
    void makeGameServicesExpanded(bool expanded);
    
    // for game completion screen styles
    void setScore(int score);
    
private:
    friend class MainMenuSceneLoader;
    CREATE_FUNC(MainMenuScene);
    MainMenuScene();
    
    CCBButton *lhsButton;
    CCBButton *rhsButton;
    CCBNodeContainer *buttonsContainer;
    cocos2d::RefPtr<cocos2d::Label> scoreLabel;
    
    bool isGameServicesExpanded = false;
    int score = 0;
};

class MainMenuSceneLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~MainMenuSceneLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(MainMenuSceneLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(MainMenuScene);
};
