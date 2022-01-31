//
//  CCBNodeContainer.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 22/02/16.
//
//

#include "CCBNodeContainer.h"

#include "cocosbuilder/CocosBuilder.h"

#include "Utils/GuiUtils.h"
#include "Resources/Resources.h"

USING_NS_CC;
using namespace cocosbuilder;

CCBNodeContainer::CCBNodeContainer() {
    
}

CCBNodeContainer::~CCBNodeContainer() {
    
}
    
bool CCBNodeContainer::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    nodes.insert(memberVariableName, node);
    return true;
}

void CCBNodeContainer::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {

}