//
//  GameDialog.h
//  Pulp
//
//  Created by Mikhail Shulepov on 03/08/16.
//
//

#pragma once

#include "UI/CCBDialog.h"

class GameDialog: public CCBDialog {
public:
    virtual ~GameDialog();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
protected:
    CREATE_FUNC(GameDialog);
    GameDialog();
};