//
//  GameScene.h
//  Pulp
//
//  Created by Mikhail Shulepov on 19/07/15.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

#include "Game/LevelDefinition.h"
#include "GameHelpers.h"

NS_CC_BEGIN
class Scene;
class Label;
NS_CC_END

class CCBButton;
class GamePhysicsHelper;
class MainMenuScene;
class BulletNode;

class GameScene
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener
, public GameContactListener {
public:
    static const int GAME_NODE_TAG = 194843;
    
    static cocos2d::Scene *createScene();
    static GameScene *createSceneForScreenshot(const cocos2d::Size &size, float scale);
    
    virtual ~GameScene();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    virtual void onEnter() override;
    virtual void onExit() override;    
    virtual void update(float dt) override;
    
    virtual void onBulletCollideWithSegment(BulletNode *bullet, Segment *segment) override;
    virtual void onBulletCollideWithCentre(BulletNode *bullet, cocos2d::Node *centre) override;
    
    void showPauseDialog();
    
    cocos2d::Label *getLevelNumberLabel() {
        return levelNumberLabel;
    }
    
protected:
    virtual void preGameOver(Segment *segment);
    void gameOver(Segment *segment);
    virtual void victory();
    void prepareLevel(const LevelDefinition &level, bool animateAppear);
    void askForSecondChance(Segment *segment);
    void resurrect();
    
    cocos2d::Vec2 getPositionForBullet(int number) const;
    void placeBulletsAtProperPositions(float duration);//animated
    void sendBullet();
    void touchOrbits();
    
    void updateTimeoutLabel();
    bool showRateDialog(cocos2d::Scene *scene);
    
    cocos2d::Color3B getColorForSegment(cocos2d::Node *segment) const;
    void saveColorForSegment(cocos2d::Node *segment);
    
    virtual cocos2d::Scene *createDefeatMenuScene() const;

    void highlightNextColors();
    
protected:
    friend class GameSceneLoader;
    CREATE_FUNC(GameScene);
    GameScene();
    
protected:
    enum class GameState {
        NONE,
        PREPARE,
        GAME,
        GAME_OVER,
        PAUSE
    };
    
    int currentLevelNumber = 0;
    
    // UI
    cocos2d::Label *levelNumberLabel;
    cocos2d::Label *timeoutLabel;
    cocos2d::Label *ringsControlHintLabel;
    cocos2d::Node *startHintLabel;
    
    // Helper nodes hierarchy
    cocos2d::Node *gameplayNode;
    cocos2d::Node *centreMarker;
    
    GamePhysicsHelper *physicsHelper;
    cocos2d::Vector<BulletNode *> bullets;
    cocos2d::Vector<Segment *> segments;
    
    GameState gameState = GameState::NONE;
    bool forScreenshot;
    
    float scaledDt = 0.01666f;
    float gameScale = 1.0f;
    
    bool useTimeout = false;
    float timeout = 0.0f;
    
    struct {
        float interval = -1.0f;
        float passedTime = 0.0f;
    } bulletsSchedule;
    
    cocos2d::Vector<BulletNode *> activeBullets;
    
    GameType gameType;
    
    std::string timeoutFormat;
    
    static cocos2d::Color3B GRAY_COLOR;
    static cocos2d::Color3B SHADOW_GRAY_COLOR;
};

class GameSceneLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~GameSceneLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(GameSceneLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(GameScene);
};
