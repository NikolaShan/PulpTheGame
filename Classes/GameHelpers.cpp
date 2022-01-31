//
//  GameNodeHelpers.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 20/07/15.
//
//

#include "GameHelpers.h"

#include "base/CCDirector.h"
#include "chipmunk/chipmunk.h"
#include "Resources/Resources.h"
#include "Utils/GuiUtils.h"
#include "AppSettings.h"

#include "Game/GameManager.h"
#include "SegmentMaker.h"
#include "Pseudo3DNode.h"
#include "BulletNode.h"

USING_NS_CC;
using namespace cocosbuilder;

#pragma mark -
#pragma mark Creator

static const cpGroup CP_GROUP_OBSTACLES = 1;
static const cpGroup CP_GROUP_ACTIVE_BULLET = 2;

static const cpBitmask CP_CATEGORY_OBSTACLES = 0b0001;
static const cpBitmask CP_CATEGORY_BULLET = 0b0010;
static const cpBitmask CP_CATEGORY_USED_BULLET = 0b0100;

static const cpCollisionType CP_COLLISION_TYPE_OBSTACLE = 0b0001;
static const cpCollisionType CP_COLLISION_TYPE_BULLET = 0b0010;
static const cpCollisionType CP_COLLISION_TYPE_USED_BULLET = 0b0100;

GameObjectCreationHelper::GameObjectCreationHelper(const cocos2d::Vec2 gravity) {
    colorScheme = GameManager::getInstance()->getColorScheme();
    physicsHelper = new GamePhysicsHelper(gravity);
}

GameObjectCreationHelper::~GameObjectCreationHelper() {
    physicsHelper->release();
}

BulletNode *GameObjectCreationHelper::createBullet(const Bullet &bulletInfo, float scale) {
    // create node
    auto nll = NodeLoaderLibrary::getInstance();
    nll->registerNodeLoader("Bullet", BulletNodeLoader::loader());
    
    CCBReader *ccbReader = new CCBReader(nll);
    const std::string file = forScreenshot ? R::UI::Share::BULLET : R::UI::Game::BULLET;
    BulletNode *bulletNode = dynamic_cast<BulletNode *>(ccbReader->readNodeGraphFromFile(UI_FILE(file)));
    if (forScreenshot) {
        bulletNode->setScale(CC_CONTENT_SCALE_FACTOR());
        bulletNode->getChildByTag(20)->setPositionY(14.0f - 1 / CC_CONTENT_SCALE_FACTOR());
    }
    ccbReader->release();
    GuiUtils::enableCascadeColorRecursively(bulletNode, true);
    GuiUtils::enableCascadeOpacityRecursively(bulletNode, true);
    
    const auto &color = colorScheme[bulletInfo.colorIdx].mainColor;
    bulletNode->setColor(Color3B(color));
    bulletNode->setBulletColor(Color3B(color));
    bulletNode->setAnchorPoint(Vec2(0.5f, 0.5f));
    
    // create physics for bullet
    const float radius = (bulletNode->getContentSize().width / 2 - 1) * scale;
    cpBody *body = cpBodyNewKinematic();
    cpShape *shape = cpCircleShapeNew(body, radius, {.x = 0.0f, .y = 0.0f});
    cpShapeSetElasticity(shape, 0.0);
    cpSpaceAddShape(physicsHelper->getSpace(), shape);
    cpSpaceAddBody(physicsHelper->getSpace(), body);
    cpBodySetUserData(body, bulletNode);
    bulletNode->setUserData(body);
    bulletNode->setUserObject(nullptr);
    
    cpShapeFilter filter = cpShapeFilterNew(CP_GROUP_OBSTACLES, CP_CATEGORY_OBSTACLES, CP_CATEGORY_USED_BULLET);
    cpShapeSetFilter(shape, filter);
    
    return bulletNode;
}

Vector<Segment *> GameObjectCreationHelper::createSegments(const OrbitDefinition &orbit, float orbitRadius) {
    Vector<Segment *> segmentNodes;
    SegmentMaker segmentMaker;
    segmentMaker.setRadius(orbitRadius - 1, orbitRadius + orbit.width);
    
    float segmentRotation = M_PI * 2 * orbit.initialRotation;
    for (const SegmentDefinition &segment: orbit.segments) {
        //create segment node
        const float segmentAngleSize = M_PI * 2 * segment.size;
        segmentMaker.setAngle(segmentAngleSize);
        
        if (segment.rotation > -1.1f) {
            segmentRotation = (orbit.initialRotation + segment.rotation) * M_PI * 2;
        }
        
        auto &color = colorScheme[segment.colorIdx].mainColor;
        auto &shadowColor = colorScheme[segment.colorIdx].shadowColor;
        const float rotationAngle = M_PI / 2 - segmentRotation - segmentAngleSize / 2;
        
        Sprite *segmentNode = segmentMaker.makeSprite();
        segmentNode->setAnchorPoint(Vec2(0.5f, 0.5f));
        
        segmentRotation += segmentAngleSize;
        Segment *segmentRepresenation = Segment::create(segmentNode, segment.size * 360.0f);
        segmentRepresenation->setSegmentColor(Color3B(color));
        segmentRepresenation->setRotation(CC_RADIANS_TO_DEGREES(rotationAngle));
        segmentRepresenation->setColor(Color3B(color));
        segmentRepresenation->setShadowColor(Color3B(shadowColor));

        segmentNodes.pushBack(segmentRepresenation);
        
        //create physics for segment
        cpBody *body = cpBodyNewKinematic();
        auto physicsPolygons = segmentMaker.makePhysicsPolygons();
        for (auto &physicsPolyPoints: physicsPolygons) {
            cpVect *verts = new cpVect[physicsPolyPoints.size()];
            for (size_t i = 0; i < physicsPolyPoints.size(); ++i) {
                verts[i].x = physicsPolyPoints[i].x;
                verts[i].y = physicsPolyPoints[i].y;
            }
            cpShape *shape = cpPolyShapeNewRaw(body, physicsPolyPoints.size(), verts, 1);
            delete [] verts;
            cpShapeSetElasticity(shape, 1.0);
            cpShapeSetFriction(shape, 0);
            
            cpSpaceAddShape(physicsHelper->getSpace(), shape);
            
            cpShapeFilter filter = cpShapeFilterNew(CP_GROUP_OBSTACLES, CP_CATEGORY_OBSTACLES, CP_CATEGORY_BULLET);
            cpShapeSetFilter(shape, filter);
            cpShapeSetCollisionType(shape, CP_COLLISION_TYPE_OBSTACLE);
        }
        cpBodySetUserData(body, segmentRepresenation);
        segmentRepresenation->setUserData(body);
        cpSpaceAddBody(physicsHelper->getSpace(), body);
    }

    return segmentNodes;
}

Node *GameObjectCreationHelper::createCentralNodeWithRadius(float radius, const Vec2 &position) {
    CCBReader *ccbReader = new CCBReader(NodeLoaderLibrary::getInstance());
    Node *centreCircle = ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Game::CENTRAL_NODE));
    ccbReader->release();
    
    centreCircle->setScale(radius * 2 / centreCircle->getContentSize().width);
    centreCircle->setPosition(position.x, position.y);
    centreCircle->setAnchorPoint(Vec2(0.5f, 0.5f));
   
    cpBody *body = cpBodyNewStatic();
    cpShape *shape = cpCircleShapeNew(body, radius, cpvzero);
    cpShapeSetElasticity(shape, 1.0);
    cpBodySetPosition(body, {.x = position.x, .y = position.y });
    cpSpaceAddShape(physicsHelper->getSpace(), shape);
    cpSpaceAddBody(physicsHelper->getSpace(), body);
    cpBodySetUserData(body, centreCircle);
    centreCircle->setUserData(body);
    
    cpShapeFilter filter = cpShapeFilterNew(CP_GROUP_OBSTACLES, CP_CATEGORY_OBSTACLES, CP_CATEGORY_BULLET);
    cpShapeSetFilter(shape, filter);
    cpShapeSetCollisionType(shape, CP_COLLISION_TYPE_OBSTACLE);
    
    return centreCircle;
}


#pragma mark -
#pragma mark Orbit Helper

void OrbitNodeHelper::update(float dt) {
    speed->update(dt);
    opacity->update(dt);
    
    const float deltaAngle = speed->getFloatValue() * dt * 360; //convert to degrees
    const GLubyte resultOpacity = 255 * opacity->getFloatValue();
    for (auto segment: segments) {
        if (segment->isActive()) {
            segment->setRotation(segment->getRotation() + deltaAngle);
            segment->setOpacity(resultOpacity);
            cpBody *body = (cpBody *)segment->getUserData();
            if (body) {
                cpBodySetAngle(body, -CC_DEGREES_TO_RADIANS(segment->getRotation()));
            }
        }
    }
}

void OrbitNodeHelper::onTouch() {
    speed->touch();
    opacity->touch();
}


#pragma mark -
#pragma mark Physics Helper

static void updateKinematicBodiesPositions(cpBody *body, GamePhysicsHelper *gamePhysics) {
    if (cpBodyGetType(body) == CP_BODY_TYPE_KINEMATIC) {
        Node *node = (Node *)cpBodyGetUserData(body);
        cpBodySetPosition(body, { .x = node->getPositionX(), .y = node->getPositionY() });
    }
}

static void updateDynamicBodiesPositions(cpBody *body, GamePhysicsHelper *gamePhysics) {
    if (cpBodyGetType(body) == CP_BODY_TYPE_DYNAMIC) {
        Node *node = (Node *)cpBodyGetUserData(body);
        cpVect pos = cpBodyGetPosition(body);
        node->setPosition(pos.x, pos.y);
      //  node->setRotation(CC_RADIANS_TO_DEGREES(cpBodyGetAngle(body)));
        
        cpVect velocity = cpBodyGetVelocity(body);
        if (velocity.y >= -0.01f && std::abs(velocity.x) < 0.01f) {
            //it's moving upward
        } else if (node->getPositionX() < 0 || node->getPositionY() < 0 || node->getPositionX() > gamePhysics->getSpaceSize().width) {
            gamePhysics->markForRemove(body);
        }
    }
}

static void removeShapesFromBodyAndSpace(cpBody *body, cpShape *shape, cpSpace *space) {
    cpSpaceRemoveShape(space, shape);
    cpShapeFree(shape);
}

static cpBool bulletToSegmentCollision(cpArbiter *arb, cpSpace *space, GamePhysicsHelper *physicsHelper) {
    if (physicsHelper->isPaused()) {
        return cpTrue;
    }
    
    cpBody *bodyBullet, *bodySegment;
    cpShape *shapeA, *shapeB;
    cpShape *shapeBullet;
    cpArbiterGetShapes(arb, &shapeA, &shapeB);
    if (cpShapeGetCollisionType(shapeA) == CP_CATEGORY_BULLET) {
        bodyBullet = cpShapeGetBody(shapeA);
        bodySegment = cpShapeGetBody(shapeB);
        shapeBullet = shapeA;
    } else {
        bodyBullet = cpShapeGetBody(shapeB);
        bodySegment = cpShapeGetBody(shapeA);
        shapeBullet = shapeB;
    }
    
    // actions taken onlu for first collision
    if (cpShapeGetCollisionType(shapeBullet) == CP_COLLISION_TYPE_BULLET) {
        const cpVect bodyVelocity = cpBodyGetVelocity(bodyBullet);
        if (std::abs(bodyVelocity.x) < 200 || bodyVelocity.y > 50) {
            Vec2 rndVelocity(rand_0_1() * 0.5, -0.1 - 0.9 * rand_0_1());
            if (bodyVelocity.x < 0) {
                rndVelocity.x = - rndVelocity.x;
            }
            if (rndVelocity.x > 0.5) {
                std::swap(rndVelocity.x, rndVelocity.y);
            }
            if (rndVelocity.x < 0.1) {
                rndVelocity.x += 0.25;
            }
            rndVelocity.normalize();
            
            const float speed = sqrtf(bodyVelocity.x * bodyVelocity.x + bodyVelocity.y * bodyVelocity.y);
            rndVelocity.x *= speed;
            rndVelocity.y *= speed;
            cpBodySetVelocity(bodyBullet, {.x = rndVelocity.x, .y = rndVelocity.y});
        }
        
        cpShapeFilter filter = cpShapeFilterNew(CP_GROUP_ACTIVE_BULLET, CP_CATEGORY_USED_BULLET, CP_CATEGORY_OBSTACLES);
        cpShapeSetFilter(shapeBullet, filter);
        cpShapeSetCollisionType(shapeBullet, CP_COLLISION_TYPE_USED_BULLET);
        
        Node *bulletNode = (Node *)cpBodyGetUserData(bodyBullet);
        physicsHelper->removeActiveBullet(bulletNode);
    }
    
    BulletNode *bullet = (BulletNode *)cpBodyGetUserData(bodyBullet);
    if (cpBodyGetType(bodySegment) == CP_BODY_TYPE_STATIC) {
        Node *center = (Node *)cpBodyGetUserData(bodySegment);
        physicsHelper->getContactListener()->onBulletCollideWithCentre(bullet, center);
    } else {
        Segment *segment = (Segment *)cpBodyGetUserData(bodySegment);
        
        SegmentCollisionData collisionData;
        collisionData.bullet = bullet;
        collisionData.segment = segment;
        
        float segmentAngleSize = segment->getAngleSize();
        float rotation = segment->getRotation();
        while (rotation < 0) {
            rotation += 360.0f;
        }
        rotation = fmodf(rotation, 360.0f);
        const float rotationFrom = fmodf(rotation - 90 - segmentAngleSize * 0.5f, 360.0f);
        const float rotationTo = fmodf(rotation - 90 + segmentAngleSize * 0.5f, 360.0f);
        collisionData.segmentAngles = std::make_pair(rotationFrom, rotationTo);
        
        physicsHelper->addStepCollision(collisionData);
    }
    
    return cpTrue;
}

GamePhysicsHelper::GamePhysicsHelper(const cocos2d::Vec2 &gravity): contactListener(nullptr) {
    spaceSize = Director::getInstance()->getWinSize();
    
    space = cpSpaceNew();
    cpSpaceSetGravity(space, {.x = gravity.x, .y =  gravity.y});
    cpSpaceSetDamping(space, 1.0);
    
    cpCollisionHandler *handler = cpSpaceAddCollisionHandler(space, CP_COLLISION_TYPE_OBSTACLE, CP_COLLISION_TYPE_BULLET);
    handler->userData = this;
    handler->postSolveFunc = (cpCollisionPostSolveFunc)bulletToSegmentCollision;
}

GamePhysicsHelper::~GamePhysicsHelper() {
    // remove bodies and shapes ?
    cpSpaceFree(space);
}

void GamePhysicsHelper::internalUpdate(float dt, bool game) {
    for (auto &&orbitHelper: orbitHelpers) {
        orbitHelper->update(dt);
    }
    cpSpaceStep(space, dt);
       
    if (!stepCollisionData.empty()) {
        SegmentCollisionData *targetCollision = nullptr;
        for (auto &&collision: stepCollisionData) {
            targetCollision = &collision;
            const auto edgeAngles = collision.segmentAngles;
            if ((edgeAngles.first < 90.0f && edgeAngles.second > 90.0f)) {
                break;
            }
        }
        contactListener->onBulletCollideWithSegment(targetCollision->bullet, targetCollision->segment);
        stepCollisionData.clear();
    }
    
    if (!bodiesToRemove.empty()) {
        for (cpBody *body: bodiesToRemove) {
            removeBody(body);
        }
        bodiesToRemove.clear();
    }
}

void GamePhysicsHelper::update(float dt, bool game) {
    if (paused) {
        return;
    }
    
    retain();
    cpSpaceEachBody(space, (cpSpaceBodyIteratorFunc)updateKinematicBodiesPositions, this);
    if (!hasActiveBullets) {
        internalUpdate(dt, game);
    } else {
        int i = 0;
        const int stepsCount = 5;
        const float scaledDt = dt / stepsCount;
        while (i < stepsCount && !paused) {
            internalUpdate(scaledDt, game);
            ++i;
        }
    }
    
    cpSpaceEachBody(space, (cpSpaceBodyIteratorFunc)updateDynamicBodiesPositions, this);
    
    release();
}

void GamePhysicsHelper::sendBullet(Node *node) {
    node->stopAllActions();
    
    // remove old kinematic body
    {
        cpBody *body = (cpBody *)node->getUserData();
        cpBodyEachShape(body, (cpBodyShapeIteratorFunc)removeShapesFromBodyAndSpace, space);
        cpSpaceRemoveBody(space, body);
        cpBodyFree(body);
    }
    
    // create dynamic physics for bullet
    cpBody *body = cpBodyNew(0.0, 0.0);
       
    cpShape *shape = cpCircleShapeNew(body, (node->getContentSize().width / 2 - 1) * gameScale, {.x = 0, .y = 0});
    cpShapeSetMass(shape, 1.0);
    cpShapeSetElasticity(shape, 1.0);
    cpShapeSetFriction(shape, 0);
    
    cpSpaceAddShape(space, shape);
    cpSpaceAddBody(space, body);
    cpBodySetPosition(body, {.x = node->getPositionX(), .y = node->getPositionY()});
    cpBodySetUserData(body, node);
    node->setUserData(body);
    
    cpShapeFilter filter = cpShapeFilterNew(CP_GROUP_ACTIVE_BULLET, CP_CATEGORY_BULLET, CP_CATEGORY_OBSTACLES);
    cpShapeSetFilter(shape, filter);
    cpShapeSetCollisionType(shape, CP_COLLISION_TYPE_BULLET);
    
    const float speed = 1100.0f * bulletsSpeed->getFloatValue();
    cpBodyApplyImpulseAtLocalPoint(body, {.x = 0.0f, .y = speed}, {.x = 0.0, .y = 0.0});
    
    bulletsSpeed->touch();
    
    addActiveBullet(node);
}

void GamePhysicsHelper::markForRemove(cpBody *body) {
    if (cpSpaceIsLocked(space)) {
        bodiesToRemove.push_back(body);
    } else {
        removeBody(body);
    }
}

void GamePhysicsHelper::removeBody(cpBody *body) {
    Node *node = (Node *)cpBodyGetUserData(body);
    if (node) {
        node->removeFromParent();
    }
    
    cpBodyEachShape(body, (cpBodyShapeIteratorFunc)removeShapesFromBodyAndSpace, space);
    cpSpaceRemoveBody(space, body);
    cpBodyFree(body);
}

void GamePhysicsHelper::removeNode(Node *node, bool removePhysicsOnly) {
    cpBody *body = (cpBody *)node->getUserData();
    node->setUserData(nullptr);
    if (removePhysicsOnly) {
        cpBodySetUserData(body, nullptr);
    }
    markForRemove(body);
}

void GamePhysicsHelper::makeBulletKinematic(cocos2d::Node *node) {
    node->stopAllActions();
    
    // remove old kinematic body
    {
        cpBody *body = (cpBody *)node->getUserData();
        cpBodyEachShape(body, (cpBodyShapeIteratorFunc)removeShapesFromBodyAndSpace, space);
        cpSpaceRemoveBody(space, body);
        cpBodyFree(body);
    }
    
    // create physics for bullet
    const float radius = (node->getContentSize().width / 2 - 1) * gameScale;
    cpBody *body = cpBodyNewKinematic();
    cpShape *shape = cpCircleShapeNew(body, radius, {.x = 0.0f, .y = 0.0f});
    cpShapeSetElasticity(shape, 0.0);
    cpSpaceAddShape(space, shape);
    cpSpaceAddBody(space, body);
    cpBodySetUserData(body, node);
    node->setUserData(body);
    node->setUserObject(nullptr);
    
    cpShapeFilter filter = cpShapeFilterNew(CP_GROUP_OBSTACLES, CP_CATEGORY_OBSTACLES, CP_CATEGORY_USED_BULLET);
    cpShapeSetFilter(shape, filter);
    
    removeActiveBullet(node);
}

void GamePhysicsHelper::addActiveBullet(cocos2d::Node *node) {
    if (std::find(activeBullets.begin(), activeBullets.end(), node) == activeBullets.end()) {
        activeBullets.push_back(node);
    }
    hasActiveBullets = (activeBullets.size() > 0);
}

void GamePhysicsHelper::removeActiveBullet(cocos2d::Node *node) {
    auto it = std::find(activeBullets.begin(), activeBullets.end(), node);
    if (it != activeBullets.end()) {
        activeBullets.erase(it);
    }
    hasActiveBullets = (activeBullets.size() > 0);
}
