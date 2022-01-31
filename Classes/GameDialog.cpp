//
//  GameDialog.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 03/08/16.
//
//

#include "GameDialog.h"

#include "cocosbuilder/CocosBuilder.h"

#include "Utils/GuiUtils.h"
#include "Resources/Resources.h"

USING_NS_CC;
using namespace cocosbuilder;

GameDialog::GameDialog() {
    designScreenSizeInDpi.width = 470.0f;
    closeOnTouchOutside = false;
    escapeClosable = true;
}

GameDialog::~GameDialog() {
    
}
    
bool GameDialog::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    return CCBDialog::onAssignCCBMemberVariable(target, memberVariableName, node);
}

void GameDialog::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
    CCBDialog::onNodeLoaded(node, nodeLoader);
}