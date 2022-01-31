//
//  SecondChanceDialog.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 18.11.15.
//
//

#include "SecondChanceDialog.h"

#include "cocosbuilder/CocosBuilder.h"

#include "UI/CCBButton.h"
#include "UI/CCBAnimationRunner.h"
#include "Utils/GuiUtils.h"
#include "Resources/Resources.h"
#include "AdMobAds.h"
#include "Utils/PLAudioEngine.h"
#include "UI/UIProgressHUD.h"
#include "AppSettings.h"

USING_NS_CC;
using namespace cocosbuilder;

SecondChanceDialog *SecondChanceDialog::createFromCcb() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("SecondChanceDialog", SecondChanceDialogLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    SecondChanceDialog *node = dynamic_cast<SecondChanceDialog *>(
        ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Game::SECOND_CHANCE_ALERT)));
    ccbReader->release();
    return node;
}

SecondChanceDialog::SecondChanceDialog(): titleNode(nullptr) {
    GuiUtils::setupEscapePressHandler(this, [this]() {
        decline();
    });
}

SecondChanceDialog::~SecondChanceDialog() {
    CC_SAFE_RELEASE(titleNode);
}
    
bool SecondChanceDialog::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    SETUP_LABEL("Title", LocalizedString(R::Strings::SecondChanceDialog::TITLE));
    SETUP_CUSTOM_CCBBUTTON("Accept", "YES", accept);
    SETUP_CUSTOM_CCBBUTTON("Decline", "NO", decline);
    ASSIGN_VARIABLE("TitleNode", titleNode, Node);
    return false;
}

void SecondChanceDialog::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
    alignTitle();
}

void SecondChanceDialog::alignTitle() {
    auto children = titleNode->getChildren();
    float maxX = 0;
    for (auto child: children) {
        maxX = std::max(maxX, child->getBoundingBox().getMaxX());
    }
    auto contentSize = titleNode->getContentSize();
    contentSize.width = maxX;
    titleNode->setContentSize(contentSize);
}

void SecondChanceDialog::accept() {
    std::function<void(void)> preshow = [this]() {
        retain();
        dismiss();
    };
    std::function<void(bool)> aftershow = [this](bool success) {
        completion(success);
        release();
    };
    
    auto rewardedVideo = Advertise::AdMobRewardedVideo::getInstance();
    if (rewardedVideo->getCurrentStatus() == Advertise::Status::LOADED) {
        preshow();
        rewardedVideo->present(aftershow);
    } else {
        auto progressHud = ProgressHUD::create();
        progressHud->setSwallowTouches(true);
        progressHud->showLoadingWithStatus("");
        progressHud->retain();
        retain();
        rewardedVideo->load([this, preshow, aftershow, progressHud, rewardedVideo](bool result) {
            if (result) {
                progressHud->dismiss();
                
                preshow();
                rewardedVideo->present(aftershow);
                
            } else {
                progressHud->showFailureWithStatus(LocalizedString(R::Strings::ERROR));
            }
            
            progressHud->release();
            release();
        });
    }
    
}

void SecondChanceDialog::decline() {
    completion(false);
    dismiss();
}

void SecondChanceDialog::dismiss() {
    if (isHiding) return;
    isHiding = true;
    
    CCBAnimationRunner::run(this, "Dismiss", [this]() {
        removeFromParent();
    });
}
