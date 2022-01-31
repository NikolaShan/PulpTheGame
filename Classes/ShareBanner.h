//
//  ShareBanner.h
//  Pulp
//
//  Created by Mikhail Shulepov on 15.12.15.
//
//

#pragma once

#include "base/CCRefPtr.h"
#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

NS_CC_BEGIN
class Label;
NS_CC_END

class ShareBanner
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    static ShareBanner *createFromCcb();
    virtual ~ShareBanner();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    void setupForLevelShare(int level);
    void setupForScoreShare(int score);
    
    void takeScreenshot(std::function<void(const std::string &filePath)> result);
    
private:
    void setupLevelInside(cocos2d::Node *node, bool endless);

    friend class ShareBannerLoader;
    CREATE_FUNC(ShareBanner);
    ShareBanner();
    
    cocos2d::RefPtr<cocos2d::Node> levelContainer;
    cocos2d::RefPtr<cocos2d::Label> scoreLabel;
    cocos2d::RefPtr<cocos2d::Label> titleLabel;
    cocos2d::RefPtr<cocos2d::Node> scene;
};

class ShareBannerLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~ShareBannerLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(ShareBannerLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(ShareBanner);
};
