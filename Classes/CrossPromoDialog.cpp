//
//  CrossPromoDialogNode.cpp
//  ImpossibleBounce
//
//  Created by Maxim Shmotin on 19/06/15.
//
//

#include "CrossPromoDialog.h"
#include "cocosbuilder/CocosBuilder.h"
#include "base/CCEventDispatcher.h"
#include "UI/CCBAnimationRunner.h"
#include "UI/WebSprite.h"
#include "Resources/Resources.h"
#include "Resources/Strings.h"
#include "Utils/GuiUtils.h"
#include "Utils/AppUtils.h"
#include "CrossPromoKit.h"
#include "UI/CCBButton.h"
#include "AppSettings.h"
#include "AnalyticsDeprecated.h"


USING_NS_CC;
using namespace cocosbuilder;

CrossPromoDialog *CrossPromoDialog::createNodeWithApp(std::shared_ptr<CrossPromoKit::AppInfo> app) {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("CrossPromoDialog", CrossPromoDialogLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    CrossPromoDialog *dialog = dynamic_cast<CrossPromoDialog*>(ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Dialogs::CROSS_PROMO_DIALOG)));
    dialog->app = app;
    dialog->setupWithApp();
    
    ccbReader->release();
    
    return dialog;
}

CrossPromoDialog::CrossPromoDialog(): nameLabel(nullptr), descriptionLabel(nullptr), dialogTitleLabel(nullptr), image(nullptr), app(nullptr){
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

CrossPromoDialog::~CrossPromoDialog() {
    CC_SAFE_RELEASE(nameLabel);
    CC_SAFE_RELEASE(descriptionLabel);
    CC_SAFE_RELEASE(dialogTitleLabel);
    CC_SAFE_RELEASE(image);
}

void CrossPromoDialog::onEnter() {
    Node::onEnter();
    dialogTitleLabel->setString(Localization::getInstance()->getString(R::Strings::CrossPromoDialog::TITLE));
    Analytics::getInstance()->sendEvent("Cross Promo", "Show Dialog", app->getEntryIdentifier());
}

void CrossPromoDialog::openAppStorePage() {
    app->showInAppStore();
    Analytics::getInstance()->sendEvent("Cross Promo", "View From Dialog", app->getEntryIdentifier());
    close();
}

void CrossPromoDialog::close() {
    if (isHiding) return;
    isHiding = true;
    
    CCBAnimationRunner::run(this, "Dismiss", [this]() {
        this->removeFromParent();
    });
}

void CrossPromoDialog::setupWithApp() {
    nameLabel->setString(app->getName());
    descriptionLabel->setString(app->getDesciption());
    image->setImageUrl(app->getUrlForResource(AppSettings::CrossPromoResourceBanner), Size(260, 136));
}

bool CrossPromoDialog::onAssignCCBMemberVariable(cocos2d::Ref* target, const std::string &memberVariableName, cocos2d::Node* node) {
    ASSIGN_VARIABLE("NameLabel", nameLabel, Label);
    ASSIGN_VARIABLE("DescriptionLabel", descriptionLabel, Label);
    ASSIGN_VARIABLE("DialogTitleLabel", dialogTitleLabel, Label);
    ASSIGN_VARIABLE("Image", image, WebSprite);
    SETUP_CUSTOM_CCBBUTTON("TouchArea", "", openAppStorePage)
    SETUP_CUSTOM_CCBBUTTON("Close", "", close)
    
    if (memberVariableName == "PlayButton") {
        CCBButton *button = dynamic_cast<CCBButton *>(node);
        button->setIcon(R::Images::Support::CHECK_IMG);
        button->setTitle(Localization::getInstance()->getString(R::Strings::CrossPromoDialog::BUTTON_TITLE));
        button->setCallback([this] {
            openAppStorePage();
        });
        return true;
    }
    
    if (memberVariableName == "ContentNode") {
        const Size sizeInDpi = AppUtils::getScreenSizeInDpi();
        if (sizeInDpi.width > 600.0f) {
            const float scale = 600.0f / sizeInDpi.width;
            node->setScale(scale);
        }
        return true;
    }
    
    
    return false;
}
