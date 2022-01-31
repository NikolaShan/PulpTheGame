//
//  CrossPromoDialogNode.h
//  ImpossibleBounce
//
//  Created by Maxim Shmotin on 19/06/15.
//
//

#ifndef __ImpossibleBounce__CrossPromoDialogNode__
#define __ImpossibleBounce__CrossPromoDialogNode__

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "CrossPromoKit.h"

NS_CC_BEGIN
class Label;
NS_CC_END
class WebSprite;

class CrossPromoDialog
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner {
public:
    static CrossPromoDialog* createNodeWithApp(std::shared_ptr<CrossPromoKit::AppInfo> app);
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref* target, const std::string &memberVariableName, cocos2d::Node* node) override;
    void setupWithApp();
    virtual void onEnter() override;
    
private:
    cocos2d::Label *nameLabel;
    cocos2d::Label *descriptionLabel;
    cocos2d::Label *dialogTitleLabel;
    WebSprite *image;
    std::shared_ptr<CrossPromoKit::AppInfo> app;
    
private:
    void openAppStorePage();
    void close();
    
private:
    friend class CrossPromoDialogLoader;
    CREATE_FUNC(CrossPromoDialog);
    CrossPromoDialog();
    virtual ~CrossPromoDialog();
    
    bool isHiding = false;
};

class CrossPromoDialogLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~CrossPromoDialogLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(CrossPromoDialogLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(CrossPromoDialog);
};

#endif /* defined(__ImpossibleBounce__CrossPromoDialogNode__) */
