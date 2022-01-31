//
//  UIProgressHUD.cpp
//  Square
//
//  Created by Mikhail Shulepov on 15/07/15.
//
//

#include "2d/CCLabel.h"
#include "2d/CCSprite.h"
#include "2d/CCLayer.h"
#include "base/CCDirector.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventDispatcher.h"
#include "cocosbuilder/CocosBuilder.h"
#include "ui/UIScale9Sprite.h"
#include "platform/CCGLView.h"

#include "UIProgressHUD.h"
#include "Utils/GuiUtils.h"
#include "Utils/AppUtils.h"
#include "UI/CCBAnimationRunner.h"

USING_NS_CC;
using namespace cocosbuilder;
using namespace ui;

class ProgressHUD::ContentNode: public Node, public CCBMemberVariableAssigner, public NodeLoaderListener {
private:
    float yPosAdjustment = 0.0f;
    float bottomMargin = 5;
    
public:
    static const std::string STATE_SUCCESS;
    static const std::string STATE_FAILURE;
    static const std::string STATE_LOADING;
    static const std::string STATE_INFO;
    
    CREATE_FUNC(ProgressHUD::ContentNode);
    CC_SYNTHESIZE_RETAIN(Label *, infoLabel, InfoLabel);
    CC_SYNTHESIZE_RETAIN(Sprite *, statusIcon, StatusIcon);
    CC_SYNTHESIZE_RETAIN(Node *, backgroundNode, BackgroundNode);
    CC_SYNTHESIZE_RETAIN(Node *, loadingIndicator, LoadingIndicator);
    
    ContentNode(): infoLabel(nullptr), statusIcon(nullptr), backgroundNode(nullptr), loadingIndicator(nullptr) {
    }
    
    virtual ~ContentNode() {
        CC_SAFE_RELEASE(infoLabel);
        CC_SAFE_RELEASE(statusIcon);
        CC_SAFE_RELEASE(backgroundNode);
        CC_SAFE_RELEASE(loadingIndicator);
    }
    
    void adjustPosition() {
        yPosAdjustment = (backgroundNode->getContentSize().height - getContentSize().height) / 2;
        
        auto parentSize = getParent()->getContentSize();
        setPositionY(parentSize.height / 2);
    }
    
    void resizeToFitContent() {
        const Vec2 minLabel(0, infoLabel->getBoundingBox().getMinY());
        Size size = backgroundNode->getBoundingBox().size;
        const float minY = backgroundNode->convertToNodeSpace( convertToWorldSpace(minLabel )).y;
        const float heightAdjustment = bottomMargin - minY;
        size.height += heightAdjustment;
        backgroundNode->setContentSize(size);
            
        adjustPosition();
    }
    
    void setMessage(const std::string &message) {
        infoLabel->setString(message);
        resizeToFitContent();
    }
    
    void setState(const std::string &state) {
        CCBAnimationRunner::run(this, state, [this]() {
            resizeToFitContent();
        });
        if (loadingIndicator && state == STATE_LOADING) {
            CCBAnimationRunner::run(loadingIndicator, "Loading", nullptr);
        }
    }
    
    virtual bool onAssignCCBMemberVariable(Ref* target, const std::string &memberVariableName, Node* node) override {
        ASSIGN_VARIABLE("Label", infoLabel, Label);
        ASSIGN_VARIABLE("Icon", statusIcon, Sprite);
        ASSIGN_VARIABLE("Background", backgroundNode, Node);
        ASSIGN_VARIABLE("LoadingIndicator", loadingIndicator, Node);
        return false;
    }
    
    virtual bool onAssignCCBCustomProperty(Ref* target, const std::string &memberVariableName, const Value& value) override {
        if (memberVariableName == "BottomMargin") {
            bottomMargin = value.asFloat();
            return true;
        }
        return false;
    };
    
    virtual void onNodeLoaded(cocos2d::Node *pNode, NodeLoader *pNodeLoader) override {
        infoLabel->setMaxLineWidth(getContentSize().width - 20);

    }
    
    virtual void setPositionY(float y) override {
        Node::setPositionY(y + yPosAdjustment);
    }
    
    virtual void setPosition(const Vec2 &pos) override {
        const Vec2 adjusted(pos.x, pos.y + yPosAdjustment);
        Node::setPosition(adjusted);
    }
    
    //check that it has timelines for all states
    void validateTimelines() {
        CCBAnimationManager *am = dynamic_cast<CCBAnimationManager *>(getUserObject());
        const auto timelines = {STATE_INFO, STATE_FAILURE, STATE_LOADING, STATE_SUCCESS};
        for (auto &&timeline: timelines) {
            const std::string errorMsg = "No timeline for ProgressHUD: " + timeline;
            CCASSERT(am->getSequenceId(timeline.c_str()) >= 0, errorMsg.c_str());
        }
    }
};

const std::string ProgressHUD::ContentNode::STATE_SUCCESS = "Success";
const std::string ProgressHUD::ContentNode::STATE_FAILURE = "Failure";
const std::string ProgressHUD::ContentNode::STATE_LOADING = "Loading";
const std::string ProgressHUD::ContentNode::STATE_INFO = "Info";


class ProgressHudLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~ProgressHudLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(ProgressHudLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(ProgressHUD::ContentNode);
};

std::string ProgressHUD::defaultCcb = "";

void ProgressHUD::setDefaultCcb(const std::string &ccbFilePath) {
    defaultCcb = ccbFilePath;
#if COCOS2D_DEBUG == 1
    //validate provided ccb
    auto hud = ProgressHUD::create();
    hud->contentNode->validateTimelines();
#endif
}

ProgressHUD *ProgressHUD::create(const std::string &ccbFilePath) {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("ProgressHUD", ProgressHudLoader::loader());
    
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    ProgressHUD::ContentNode *contentNode = dynamic_cast<ProgressHUD::ContentNode *>
        (ccbReader->readNodeGraphFromFile(ccbFilePath.c_str()));
    ccbReader->release();
    
    auto ret = new ProgressHUD(contentNode);
    ret->autorelease();
    return ret;
}

ProgressHUD *ProgressHUD::create() {
    CC_ASSERT(!defaultCcb.empty());
    return create(defaultCcb);
}

ProgressHUD::ProgressHUD(ContentNode *pContentNode): rootNode(nullptr) {
    const Size winSize = Director::getInstance()->getWinSize();
    blackoutNode = LayerColor::create(Color4B(0, 0, 0, 1), winSize.width, winSize.height);
    blackoutNode->retain();
    blackoutNode->setPosition(Vec2::ZERO);
    blackoutNode->setAnchorPoint(Vec2::ZERO);
    
    setContentSize(winSize);
    addChild(blackoutNode);
    
    contentNode = pContentNode;
    contentNode->retain();
    
    contentNode->setAnchorPoint(Vec2(0.5f, 0.5f));
    
    Node *contentHolder = Node::create();
    contentHolder->setAnchorPoint(Vec2(0.5f, 0.5f));
    contentHolder->setPositionX(winSize.width / 2);
    contentHolder->setPositionY(winSize.height / 2);
    contentHolder->addChild(contentNode);
    
    GuiUtils::enableCascadeOpacityRecursively(contentHolder, true);
    
    blackoutNode->addChild(contentHolder);
    
    touchHandler = EventListenerTouchOneByOne::create();
    touchHandler->onTouchBegan = [this](Touch *touch, Event *event) {
        if (dismissOnTouch) {
            dismiss();
        }
        return true;
    };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchHandler, blackoutNode);
    touchHandler->retain();
    
    GuiUtils::setupEscapePressHandler(blackoutNode, []() {
        //just swallow
    });   
}

ProgressHUD::~ProgressHUD() {
    CC_SAFE_RELEASE(blackoutNode);
    CC_SAFE_RELEASE(contentNode);
    CC_SAFE_RELEASE(touchHandler);
}

void ProgressHUD::setSwallowTouches(bool swallow) {
    touchHandler->setSwallowTouches(swallow);
}

void ProgressHUD::setDissmissOnTouch(bool shouldDismiss) {
    dismissOnTouch = shouldDismiss;
    touchHandler->setSwallowTouches(true);
}

void ProgressHUD::setBlackout() {
    blackout = true;
    setSwallowTouches(true);
}

void ProgressHUD::setPositionGravity(PositionGravity gravity) {
    //TODO:
}

void ProgressHUD::setExactPosition(float yPosition) {
    //TODO:
}

//it will be automatically removed from screen after delay
void ProgressHUD::showSuccessWithStatus(const std::string &status) {
    contentNode->setState(ContentNode::STATE_SUCCESS);
    contentNode->setMessage(status);
    presentAndRunAppearAnimations();
    scheduleDismiss();
}

void ProgressHUD::showFailureWithStatus(const std::string &status) {
    contentNode->setMessage(status);
    contentNode->setState(ContentNode::STATE_FAILURE);
    presentAndRunAppearAnimations();
    scheduleDismiss();
}

void ProgressHUD::showInfoWithStatus(const std::string &status) {
    contentNode->setMessage(status);
    contentNode->setState(ContentNode::STATE_INFO);
    presentAndRunAppearAnimations();
    scheduleDismiss();
}

void ProgressHUD::showLoadingWithStatus(const std::string &status) {
    contentNode->setMessage(status);
    contentNode->setState(ContentNode::STATE_LOADING);
    presentAndRunAppearAnimations();
}

void ProgressHUD::presentAndRunAppearAnimations() {
    if (getParent()) {
        return;
    }
    if (rootNode) {
        rootNode->addChild(this);
    } else {
        Director::getInstance()->getRunningScene()->addChild(this);
    }
    rootNode = nullptr;
    
    if (blackout) {
        blackoutNode->runAction(FadeTo::create(0.15, 100));
    }
    
    const float designWidth = Director::getInstance()->getOpenGLView()->getDesignResolutionSize().width;
    const float screenWidth = AppUtils::getScreenSizeInDpi().width;
    const float resultScale = std::min(1.0f, std::max(0.5f, designWidth / screenWidth));
    
    Node *contentHolder = contentNode->getParent();
    contentHolder->setScale(resultScale * 1.15f);
    contentHolder->setOpacity(0);
    
    auto delay = DelayTime::create(0.05f);
    auto spawn = Spawn::create(ScaleTo::create(0.15f, resultScale), FadeTo::create(0.15f, 255), nullptr);
    contentHolder->runAction(Sequence::create(delay, spawn, nullptr));
    
    touchHandler->setEnabled(true);
}

void ProgressHUD::runDisappearAnimationsAndRemove() {
    touchHandler->setEnabled(false);
    if (!getParent()) {
        return;
    }
    
    blackoutNode->stopAllActions();
    blackoutNode->runAction(FadeTo::create(0.15f, 0));
    
    auto removeCallfunc = RemoveSelf::create();
    runAction(Sequence::create(DelayTime::create(0.15f), removeCallfunc, nullptr));
    
    Node *contentHolder = contentNode->getParent();
    contentHolder->stopAllActions();
    contentHolder->runAction(ScaleBy::create(0.15f, 1.15));
    contentHolder->runAction(FadeTo::create(0.15f, 0));
}

void ProgressHUD::dismiss() {
    runDisappearAnimationsAndRemove();
}

void ProgressHUD::dismissAfter(float delay) {
    if (!dismissScheduled) {
        dismissScheduled = true;
    }
    scheduleOnce([this](float ) {
        dismiss();
    }, delay, "Dismiss");
}

void ProgressHUD::scheduleDismiss() {
    const size_t msgLen = contentNode->getInfoLabel()->getString().size();
    const float delay = 1.5f + msgLen * 0.04f;
    dismissAfter(delay);
}
