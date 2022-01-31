//
//  AppTurboDialog.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 06/03/16.
//
//

#include "AppTurboDialog.h"

#include "base/CCEventDispatcher.h"
#include "base/CCDirector.h"
#include "cocosbuilder/CocosBuilder.h"

#include "UI/CCBAnimationRunner.h"
#include "UI/CCBButton.h"
#include "Utils/AppUtils.h"

#include "Utils/GuiUtils.h"
#include "Resources/Resources.h"

USING_NS_CC;
using namespace cocosbuilder;

AppTurboDialog *AppTurboDialog::createFromCcb() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("AppTurboDialog", AppTurboDialogLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    AppTurboDialog *node = dynamic_cast<AppTurboDialog *>(ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Dialogs::APP_TURBO)));
    ccbReader->release();
    return node;
}

AppTurboDialog::AppTurboDialog() {
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

AppTurboDialog::~AppTurboDialog() {
    
}
    
bool AppTurboDialog::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    if (memberVariableName == "ContentNode") {
        const auto sizeInDpi = AppUtils::getScreenSizeInDpi();
        if (sizeInDpi.width > 470.0f) {
            const float scale = 470.0f / sizeInDpi.width;
            node->setScale(scale);
        }
        return true;
    }
    SETUP_LABEL("Title", LocalizedString(R::Strings::AppTurbo::TITLE))
    SETUP_LABEL("Message", LocalizedString(R::Strings::AppTurbo::MESSAGE))
    SETUP_CUSTOM_CCBBUTTON("OkayButton", LocalizedString(R::Strings::AppTurbo::BUTTON), dismiss)
    return false;
}

void AppTurboDialog::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {

}

void AppTurboDialog::dismiss() {
    if (isHiding) return;
    isHiding = true;
    
    CCBAnimationRunner::run(this, "Dismiss", [this]() {
        removeFromParent();
    });
}
