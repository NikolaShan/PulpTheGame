//
//  RateDialog.cpp
//  Pulp
//
//  Created by Maxim Shmotin on 01/08/15.
//
//

#include "RateDialog.h"
//
//  RateDialog
//  Pulp
//
//  Created by Maxim Shmotin on 01/08/15.
//
//

#include "CrossPromoDialog.h"
#include "cocosbuilder/CocosBuilder.h"
#include "base/CCEventDispatcher.h"
#include "platform/CCApplication.h"
#include "UI/CCBAnimationRunner.h"
#include "UI/WebSprite.h"
#include "Resources/Resources.h"
#include "Resources/Strings.h"
#include "Utils/GuiUtils.h"
#include "Utils/AppUtils.h"
#include "Utils/RateScheduler.h"
#include "UI/CCBButton.h"
#include "AppSettings.h"
#include "Analytics.h"


USING_NS_CC;
using namespace cocosbuilder;

RateDialog *RateDialog::createNode() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("RateDialog", RateDialogLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    RateDialog *dialog = dynamic_cast<RateDialog*>(ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Dialogs::RATE_DIALOG)));
    ccbReader->release();
    
    return dialog;
}

RateDialog::RateDialog(){
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        return true;
    };
    listener->setSwallowTouches(true);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    GuiUtils::setupEscapePressHandler(this, [this]() {
        close();
    });
}

RateDialog::~RateDialog() {
}

void RateDialog::onEnter() {
    Node::onEnter();
}

void RateDialog::openAppStorePage() {
    Analytics::getInstance()->sendEvent("rate", {{"from", "dialog"}});
    Application::getInstance()->openURL(AppSettings::getInstance()->getRateUrl());
    close();
    RateScheduler::getInstance()->setRatedThisVersion();
}

void RateDialog::decline() {
    RateScheduler::getInstance()->setDeclinedThisVersion();
    close();
}

void RateDialog::close() {
    if (isHiding) return;
    isHiding = true;
    
    CCBAnimationRunner::run(this, "Dismiss", [this]() {
        this->removeFromParent();
    });
}

bool RateDialog::onAssignCCBMemberVariable(cocos2d::Ref* target, const std::string &memberVariableName, cocos2d::Node* node) {  
    SETUP_LABEL("DialogTitleLabel", Localization::getInstance()->getString(R::Strings::RateDialog::TITLE));
    SETUP_LABEL("DescriptionLabel", Localization::getInstance()->getString(R::Strings::RateDialog::MESSAGE));

    SETUP_CUSTOM_CCBBUTTON("RateButton", Localization::getInstance()->getString(R::Strings::RateDialog::BUTTON_TITLE), openAppStorePage)
    SETUP_CUSTOM_CCBBUTTON("LaterButton", LocalizedString(R::Strings::RateDialog::REMIND_LATER), close)
    SETUP_CUSTOM_CCBBUTTON("DeclineButton", LocalizedString(R::Strings::RateDialog::DECLINE), decline)
    
    if (memberVariableName == "ContentNode") {
        const Size sizeInDpi = AppUtils::getScreenSizeInDpi();
        if (sizeInDpi.width > 500.0f) {
            const float scale = 500.0f / sizeInDpi.width;
            node->setScale(scale);
        }
        return true;
    }
    
    return false;
}
