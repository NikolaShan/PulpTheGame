//
//  PauseDialog.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 18/02/16.
//
//

#include "PauseDialog.h"

#include "cocosbuilder/CocosBuilder.h"

#include "UI/CCBButton.h"
#include "UI/CCBAnimationRunner.h"
#include "Utils/GuiUtils.h"
#include "Resources/Resources.h"
#include "UI/UIProgressHUD.h"
#include "AppSettings.h"

USING_NS_CC;
using namespace cocosbuilder;

PauseDialog *PauseDialog::createFromCcb() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("PauseDialog", PauseDialogLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    PauseDialog *node = dynamic_cast<PauseDialog *>(
        ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Dialogs::PAUSE_DIALOG)));
    ccbReader->release();
    return node;
}

PauseDialog::PauseDialog() {
    GuiUtils::setupEscapePressHandler(this, [this]() {
        resumeGame();
    });
}

PauseDialog::~PauseDialog() {

}

bool PauseDialog::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    SETUP_LABEL("Title", LocalizedString(R::Strings::PauseDialog::TITLE));
    SETUP_CUSTOM_CCBBUTTON("ExitButton", LocalizedString("PauseDialog/MenuButton"), exitToMenu);
    SETUP_CUSTOM_CCBBUTTON("ResumeButton", LocalizedString("PauseDialog/ResumeButton"), resumeGame);
    return false;
}

void PauseDialog::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {

}

void PauseDialog::exitToMenu() {
    completion(false);
}

void PauseDialog::resumeGame() {
    if (isHiding) return;
    isHiding = true;
    
    CCBAnimationRunner::run(this, "Dismiss", [this]() {
        completion(true);
        removeFromParent();
    });
}
