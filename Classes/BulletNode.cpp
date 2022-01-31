//
//  BulletNode.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 09/06/16.
//
//

#include "BulletNode.h"

#include "cocosbuilder/CocosBuilder.h"

#include "Utils/GuiUtils.h"
#include "Resources/Resources.h"
#include "AppSettings.h"

USING_NS_CC;
using namespace cocosbuilder;

BulletNode *BulletNode::createFromCcb() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("BulletNode", BulletNodeLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    BulletNode *node = dynamic_cast<BulletNode *>(ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Game::BULLET)));
    ccbReader->release();
    return node;
}

BulletNode::BulletNode() {
    
}

BulletNode::~BulletNode() {
    CC_SAFE_RELEASE(nail);
}
    
bool BulletNode::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    return false;
}

void BulletNode::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
    //scheduleUpdate();
    
   /* nail = Sprite::create(R::Images::Support::SQUARE_8_IMG);
    nail->retain();
    nail->setAnchorPoint(Vec2(0.0f, 0.5f));
    nail->setPosition(getContentSize().width * 0.5f, getContentSize().height * 0.5f);
    addChild(nail);*/
}


void BulletNode::update(float dt) {
    Node::update(dt);
    
    /*const Vec2 deltaPos = prevPos - getPosition();
    const float speed = deltaPos.getLength() / dt;
    if (speed > FLT_EPSILON) {
        nail->setVisible(true);
    
        const float r = getContentSize().width * 0.5f;
        const float len = r + std::min(10.0f, speed / 40);
        const float alpha = acosf(r / len);
        const float h = r * sinf(alpha);
        const float d = r * cosf(alpha);
        
        PolygonInfo &polyInfo = nail->getPolygonInfo();
        V3F_C4B_T2F *verts = polyInfo.triangles.verts;
        
        const Point p0(d, h);
        
        verts[1].vertices = Vec3(0.0f, h, 0.0f);
        verts[0].vertices = Vec3(p0.x, p0.y + h, 0.0f);
        verts[3].vertices = Vec3(p0.x, 0.0f, 0.0f);
        verts[2].vertices = Vec3(len, h, 0.0f);
        
        nail->setContentSize(Size(len, 2 * h));
        
        const float rotation = deltaPos.getAngle(Vec2(1.0f, 0.0f));
        nail->setRotation(CC_RADIANS_TO_DEGREES(rotation));
        
    } else {
        nail->setVisible(false);
    }
    
    prevPos = getPosition();*/
}
