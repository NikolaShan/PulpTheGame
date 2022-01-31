//
//  Pseudo3DNode.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 24/07/15.
//
//

#include "Pseudo3DNode.h"

#include "cocosbuilder/CocosBuilder.h"

#include "Utils/GuiUtils.h"
#include "Resources/Resources.h"

USING_NS_CC;
using namespace cocosbuilder;

Pseudo3DNode *Pseudo3DNode::create(cocos2d::Node *mainNode, cocos2d::Node *shadowNode, float offset) {
    Pseudo3DNode *node = Pseudo3DNode::create();
    node->setContentSize(mainNode->getContentSize());

    shadowNode->setAnchorPoint(Vec2::ZERO);
    shadowNode->setPosition(Vec2::ZERO);
    node->addChild(shadowNode);
    node->shadowNode = shadowNode;
    node->shadowNode->retain();
    node->originalShadowColor = shadowNode->getColor();
    
    mainNode->setAnchorPoint(Vec2::ZERO);
    mainNode->setPosition(Vec2::ZERO);
    node->addChild(mainNode);
 
    node->offset = offset;
    
    node->setContentSize(mainNode->getContentSize());
    node->updateShadowPosition();
    
    return node;
}

Pseudo3DNode::Pseudo3DNode(): shadowNode(nullptr) {
    
}

Pseudo3DNode::~Pseudo3DNode() {
    CC_SAFE_RELEASE(shadowNode);
}
    
bool Pseudo3DNode::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    ASSIGN_VARIABLE("Shadow", shadowNode, Node);
    return false;
}

bool Pseudo3DNode::onAssignCCBCustomProperty(Ref* target, const std::string &memberVariableName, const Value& value) {
    if (memberVariableName == "Offset") {
        offset = value.asFloat();
        return true;
    }
    return false;
}

void Pseudo3DNode::onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) {
    shadowNode->setAnchorPoint(Vec2::ZERO);
    shadowNode->setPosition(Vec2::ZERO);
}

void Pseudo3DNode::setRotation(float rotation) {
    Node::setRotation(rotation);
    updateShadowPosition();
}

void Pseudo3DNode::onExit() {
    Node::onExit();
    if (extracted) {
        shadowNode->setVisible(false);
    }
}

void Pseudo3DNode::removeFromParentAndCleanup(bool cleanup) {
    if (extracted) {
        shadowNode->removeFromParent();
    }
    Node::removeFromParentAndCleanup(cleanup);
}

void Pseudo3DNode::setColor(const cocos2d::Color3B &color) {
    Node::setColor(color);
    updateShadowColor();
}

void Pseudo3DNode::setOpacity(GLubyte opacity) {
    Node::setOpacity(opacity);
    updateShadowOpacity();
}

void Pseudo3DNode::updateShadowColor() {
    if (extracted && isCascadeColorEnabled()) {
        const Color3B selfColor = getColor();
        Color3B displayedColor;
        displayedColor.r = 1.0 * selfColor.r * originalShadowColor.r/255.0;
        displayedColor.g = 1.0 * selfColor.g * originalShadowColor.g/255.0;
        displayedColor.b = 1.0 * selfColor.b * originalShadowColor.b/255.0;
        shadowNode->setColor(displayedColor);
    }
}

void Pseudo3DNode::updateShadowOpacity() {
    if (extracted && isCascadeOpacityEnabled()) {
        shadowNode->setOpacity(getOpacity());
    }
}

void Pseudo3DNode::extractShadow() {
    extracted = true;
    shadowNode->removeFromParent();
    getParent()->addChild(shadowNode);
    shadowNode->setPosition(getPosition());
    shadowNode->setAnchorPoint(getAnchorPoint());
    shadowNode->setRotation(getRotation());
    shadowNode->setLocalZOrder(9);
    setLocalZOrder(10);
    updateShadowPosition();
    updateShadowColor();
    updateShadowOpacity();
}

void Pseudo3DNode::updateShadowPosition() {
    const float rotation = getRotation();
    if (extracted) {
        shadowNode->setRotation(rotation);
        shadowNode->setScale(getScale());
    }
}