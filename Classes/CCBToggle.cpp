//
//  CCBToggle.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 04/08/16.
//
//

#include "CCBToggle.h"

#include "cocosbuilder/CocosBuilder.h"

#include "Utils/GuiUtils.h"
#include "Resources/Resources.h"

USING_NS_CC;
using namespace cocosbuilder;

CCBToggle::CCBToggle() {
    
}

CCBToggle::~CCBToggle() {
    
}
    
bool CCBToggle::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    return CCBButton::onAssignCCBMemberVariable(target, memberVariableName, node);
}

void CCBToggle::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
    CCBButton::onNodeLoaded(node, nodeLoader);
}

void CCBToggle::setSelected(bool selected, bool animated) {
    const float duration = animated ? 0.2f : 0.0f;
    const std::string state = selected ? "Selected" : "Deselected";
    animateToState(state, duration);
}
