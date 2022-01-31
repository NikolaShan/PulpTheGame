//
//  ExitConfirmationDialog.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 15.12.15.
//
//

#include "ExitConfirmationDialog.h"

#include "base/CCEventDispatcher.h"
#include "base/CCDirector.h"
#include "cocosbuilder/CocosBuilder.h"

#include "Utils/GuiUtils.h"
#include "Utils/AppUtils.h"
#include "Utils/Localization.h"
#include "UI/CCBAnimationRunner.h"
#include "UI/CCBButton.h"

#include "Resources/Resources.h"
#include "Resources/Strings.h"
#include "AppSettings.h"

USING_NS_CC;
using namespace cocosbuilder;

ExitConfirmationDialog *ExitConfirmationDialog::createFromCcb() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("ExitConfirmationDialog", ExitConfirmationDialogLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    ExitConfirmationDialog *node = dynamic_cast<ExitConfirmationDialog *>(ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Dialogs::EXIT_CONFIRMATION_DIALOG)));
    ccbReader->release();
    return node;
}

ExitConfirmationDialog::ExitConfirmationDialog() {
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        return true;
    };
    listener->setSwallowTouches(true);
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    GuiUtils::setupEscapePressHandler(this, [this]() {
        dismiss();
    });
}

ExitConfirmationDialog::~ExitConfirmationDialog() {
    
}
    
bool ExitConfirmationDialog::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    if (memberVariableName == "ContentNode") {
        const auto sizeInDpi = AppUtils::getScreenSizeInDpi();
        if (sizeInDpi.width > 470.0f) {
            const float scale = 470.0f / sizeInDpi.width;
            node->setScale(scale);
        }
        return true;
    }
    
    SETUP_CUSTOM_CCBBUTTON("YesButton", LocalizedString(R::Strings::ExitConfirmationDialog::BUTTON_EXIT_GAME), exitGame);
    SETUP_CUSTOM_CCBBUTTON("NoButton", LocalizedString(R::Strings::ExitConfirmationDialog::BUTTON_RESUME_GAME), dismiss);
    SETUP_LABEL("TitleLabel", LocalizedString(R::Strings::ExitConfirmationDialog::TITLE));
    SETUP_LABEL("MessageLabel", LocalizedString(R::Strings::ExitConfirmationDialog::MESSAGE));
    
    return false;
}

void ExitConfirmationDialog::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {

}

void ExitConfirmationDialog::dismiss() {
    if (isHiding) return;
    isHiding = true;
    
    CCBAnimationRunner::run(this, "Dismiss", [this]() {
        removeFromParent();
    });
}

void ExitConfirmationDialog::exitGame() {
    Director::getInstance()->end();
}
