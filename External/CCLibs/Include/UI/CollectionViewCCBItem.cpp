//
//  CollectionViewCCBItem.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 23/07/15.
//
//

#include "CollectionViewCCBItem.h"

#include "cocosbuilder/CocosBuilder.h"

#include "Utils/GuiUtils.h"

USING_NS_CC;
using namespace cocosbuilder;

CollectionViewCCBItem *CollectionViewCCBItem::createFromCcb(const std::string &ccb, const cocos2d::Size &size) {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("CollectionViewCCBItem", CollectionViewCCBItemLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    CollectionViewCCBItem *node = dynamic_cast<CollectionViewCCBItem *>(ccbReader->readNodeGraphFromFile(ccb.c_str(), nullptr, size));
    node->ccbFile = ccb;
    node->size = size;
    node->animationManager = dynamic_cast<CCBAnimationManager *>(node->getUserObject());
    node->animationManager->retain();
    ccbReader->release();
    return node;
}

CollectionViewCCBItem::CollectionViewCCBItem(): animationManager(nullptr) {
    
}

CollectionViewCCBItem::~CollectionViewCCBItem() {
    CC_SAFE_RELEASE(animationManager);
}
    
bool CollectionViewCCBItem::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    nodes.insert(memberVariableName, node);
    return true;
}

CollectionViewItem *CollectionViewCCBItem::createCloneInstance() const {
    return CollectionViewCCBItem::createFromCcb(ccbFile, size);
}

void CollectionViewCCBItem::runAnimation(const std::string &animation, float tweenDuration) {
    int animationId = animationManager->getSequenceId(animation.c_str());
    if (animationId >= 0) {
        animationManager->runAnimationsForSequenceIdTweenDuration(animationId, tweenDuration);
    }
}

void CollectionViewCCBItem::updateSelectionState(bool selected, bool animated) {
    const std::string animationName = selected ? "Selected" : "Normal";
    const float duration = animated ? 0.05f : 0.0f;
    runAnimation(animationName, duration);
}