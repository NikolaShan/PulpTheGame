//
//  SkipLevelDialog.h
//  Pulp
//
//  Created by Mikhail Shulepov on 26/07/15.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

NS_CC_BEGIN
class Scene;
class Label;
NS_CC_END

class CCBButton;

class SkipLevelDialog
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener {
public:
    static void showWithCompletion(std::function<void(bool skipped)> completion, cocos2d::Node *scene);
    static void showSkipForVideo(std::function<void(bool skipped)> completion, cocos2d::Node *scene);
    static void showWithCcb(const std::string &ccbFile, std::function<void (bool skipped)> completion, cocos2d::Node *scene);
    
    static bool canSkipForRewardedVideo();
    
    virtual ~SkipLevelDialog();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
private:
    static void showSkipWithOptions(std::function<void (bool skipped)> completion, cocos2d::Node *scene);
    void dismiss();
    void performLevelSkip(const std::string &source);
    
    void cancel();
    void skipForSkips();
    void buySkips1();
    void buySkips3();
    void buySkips(const std::string &name);

    void skipForRewardedVideo();
    void skipForShareInVkontakte();
    void skipForShareInFacebook();
    void skipForShareInTwitter();
    void skipForInviteFriend();
    
    bool canShareTo(const std::string &key) const;
    
    static const std::string SHARE_TO_FB_KEY;
    static const std::string SHARE_TO_VK_KEY;
    static const std::string INVITE_FB_KEY;
    static const std::string SHARE_TO_TWITTER_KEY;
    
private:
    friend class SkipLevelDialogLoader;
    CREATE_FUNC(SkipLevelDialog);
    SkipLevelDialog();
    
    bool canSkipForInviteFriends() const;
    std::string getTitleForBuySkips(const std::string &name) const;
    void performCompletionWithResult(bool result);
    
private:
    std::function<void(bool skipped)> completion;
    
    bool isHiding = false;
};

class SkipLevelDialogLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~SkipLevelDialogLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(SkipLevelDialogLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(SkipLevelDialog);
};
