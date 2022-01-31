//
//  GameNodeHelpers.h
//  Pulp
//
//  Created by Mikhail Shulepov on 20/07/15.
//
//

#pragma once

#include "2d/CCNode.h"
#include "Game/LevelDefinition.h"
#include "cocosbuilder/CocosBuilder.h"
#include "chipmunk/chipmunk.h"
#include "Segment.h"
#include "Game/GameManager.h"

class GamePhysicsHelper;
class BulletNode;

class GameObjectCreationHelper {
public:
    GameObjectCreationHelper(const cocos2d::Vec2 gravity);
    ~GameObjectCreationHelper();
    
    CC_SYNTHESIZE(bool, forScreenshot, ForScreenshot);
    
    cocos2d::Vector<Segment *> createSegments(const OrbitDefinition &orbit, float orbitRadius);
    BulletNode *createBullet(const Bullet &bulletInfo, float scale);
    cocos2d::Node *createCentralNodeWithRadius(float radius, const cocos2d::Vec2 &position);
    
    GamePhysicsHelper *getPhysicsHelper() const {
        return physicsHelper;
    }
    
private:
    std::vector<SegmentColorInfo> colorScheme;
    
    GamePhysicsHelper *physicsHelper;
};

class OrbitNodeHelper: public cocos2d::Ref {
public:
    
    static OrbitNodeHelper *create() {
        auto ret = new OrbitNodeHelper;
        ret->autorelease();
        return ret;
    }
    
    CC_SYNTHESIZE(DynamicValuePtr, speed, Speed);
    CC_SYNTHESIZE(DynamicValuePtr, opacity, Opacity);
    
    void addSegment(Segment *segment) {
        segments.pushBack(segment);
    }
       
    void update(float dt);
    
    void onTouch();
    
    cocos2d::Vector<Segment *> getSegments() const {
        return segments;
    }
    
private:
    OrbitNodeHelper() {}
    cocos2d::Vector<Segment *> segments;
};

class GameContactListener {
public:
    virtual void onBulletCollideWithSegment(BulletNode *bullet, Segment *segment) = 0;
    virtual void onBulletCollideWithCentre(BulletNode *bullet, cocos2d::Node *centre) = 0;
};

struct SegmentCollisionData {
    BulletNode *bullet;
    Segment *segment;
    std::pair<float, float> segmentAngles;
};

class GamePhysicsHelper: public cocos2d::Ref {
public:
    GamePhysicsHelper(const cocos2d::Vec2 &gravity);
    ~GamePhysicsHelper();
    
    cpSpace *getSpace() const { return space; }
       
    void update(float dt, bool game);
    void sendBullet(cocos2d::Node *node);
    
    void markForRemove(cpBody *body);
    void removeNode(cocos2d::Node *node, bool removePhysicsOnly);
    
    CC_SYNTHESIZE(GameContactListener *, contactListener, ContactListener);
    CC_SYNTHESIZE(DynamicValuePtr, bulletsSpeed, BulletsSpeed);
    CC_SYNTHESIZE(cocos2d::Vector<OrbitNodeHelper *>, orbitHelpers, OrbitHelpers);
    CC_SYNTHESIZE(float, gameScale, GameScale);
    
    const cocos2d::Size &getSpaceSize() const {
        return spaceSize;
    }
    
    bool isPaused() const {
        return paused;
    }
    
    void pause() {
        paused = true;
    }
    
    void resume() {
        paused = false;
    }
    
    void addActiveBullet(cocos2d::Node *node);
    void removeActiveBullet(cocos2d::Node *node);
    
    void addStepCollision(const SegmentCollisionData &data) {
        stepCollisionData.push_back(data);
    }
    
    void makeBulletKinematic(cocos2d::Node *bullet);
        
private:
    void internalUpdate(float dt, bool game);
    void removeBody(cpBody *body);
    
private:
    std::vector<cpBody *> bodiesToRemove;
    cpSpace *space;
    cocos2d::Size spaceSize;
    bool paused = false;
    bool hasActiveBullets = false;
    std::vector<cocos2d::Node *> activeBullets;
    std::vector<SegmentCollisionData> stepCollisionData;
};
