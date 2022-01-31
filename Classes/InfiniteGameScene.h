//
//  InfiniteGameScene.hpp
//  Pulp
//
//  Created by Mikhail Shulepov on 05/06/16.
//
//

#pragma once

#include "GameScene.h"

class InfiniteGameScene: public GameScene {
public:
    static cocos2d::Scene *createScene();
    
    virtual ~InfiniteGameScene() {}
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    void prepareNextLevel(bool animateAppear);
    virtual void onBulletCollideWithSegment(BulletNode *bullet, Segment *segment) override;
    
protected:
    virtual void preGameOver(Segment *segment) override;
    virtual void victory() override;
    void setup();
    virtual cocos2d::Scene *createDefeatMenuScene() const override;

    friend class InfiniteGameSceneLoader;
    CREATE_FUNC(InfiniteGameScene);
    InfiniteGameScene() {}
    
private:
    int attemptsCount = 0;
    int score = 0;
    
    cocos2d::Vector<cocos2d::Node *> hearts;
};

class InfiniteGameSceneLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~InfiniteGameSceneLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(InfiniteGameSceneLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(InfiniteGameScene);
};