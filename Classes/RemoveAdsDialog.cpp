//
//  RemoveAdsDialog.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 22/07/15.
//
//

#include "RemoveAdsDialog.h"

#include "2d/CCLabel.h"
#include "2d/CCScene.h"
#include "cocosbuilder/CocosBuilder.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerTouch.h"

#include "UI/CCBButton.h"
#include "Utils/GuiUtils.h"
#include "Utils/AppUtils.h"
#include "Resources/Resources.h"
#include "Resources/Strings.h"
#include "UI/CCBAnimationRunner.h"
#include "IAPurchasesManager.h"
#include "AnalyticsDeprecated.h"
#include "AppSettings.h"

USING_NS_CC;
using namespace cocosbuilder;

void RemoveAdsDialog::show() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("RemoveAdsDialog", RemoveAdsDialogLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    Node *rootNode = ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Dialogs::REMOVE_ADS));
    ccbReader->release();
    
    auto scene = Director::getInstance()->getRunningScene();
    scene->addChild(rootNode);
}

RemoveAdsDialog::RemoveAdsDialog() {
    GuiUtils::setupEscapePressHandler(this, [this]() {
        cancel();
    });
}

RemoveAdsDialog::~RemoveAdsDialog() {
    
}
    
bool RemoveAdsDialog::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    SETUP_LABEL_MAX_WIDTH("TitleLabel", LocalizedString(R::Strings::RemoveAdsDialog::TITLE), 270.0f);
    SETUP_LABEL("MessageLabel", LocalizedString(R::Strings::RemoveAdsDialog::MESSAGE));
    
    SETUP_CUSTOM_CCBBUTTON("BuyButton", getBuyButtonTitle(), buy);
    SETUP_CUSTOM_CCBBUTTON("RestoreButton", LocalizedString(R::Strings::RemoveAdsDialog::RESTORE), restore);
    SETUP_CUSTOM_CCBBUTTON("CancelButton", LocalizedString(R::Strings::RemoveAdsDialog::CANCEL), cancel);
    
    if (memberVariableName == "ContentNode") {
        const Size sizeInDpi = AppUtils::getScreenSizeInDpi();
        if (sizeInDpi.width > 470.0f) {
            const float scale = 470.0f / sizeInDpi.width;
            node->setScale(scale);
        }
        return true;
    }
    
    return false;
}

void RemoveAdsDialog::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch *touch, Event *event) {
        return true;
    };    
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

std::string RemoveAdsDialog::getBuyButtonTitle() const {
    auto product = IA::PurchasesManager::getInstance()->getProduct("no_ads");
    std::string price;
    if (product) {
        price = product->getLocalizedPrice();
    } else {
        price = "---";
    }
    return FormattedLocalizedString(R::Strings::RemoveAdsDialog::BUY, price);
}

void RemoveAdsDialog::cancel() {
    if (isHiding) return;
    isHiding = true;
    
    CCBAnimationRunner::run(this, "Dismiss", [this]() {
        removeFromParent();
    });
}

void RemoveAdsDialog::buy() {
    IA::PurchasesManager::getInstance()->buyProduct("no_ads", [](bool success) {
    });
    cancel();
}

void RemoveAdsDialog::restore() {
    IA::PurchasesManager::getInstance()->restorePurchases();
    cancel();
}
