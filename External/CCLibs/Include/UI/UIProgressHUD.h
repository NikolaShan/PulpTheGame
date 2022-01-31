//
//  UIProgressHUD.h
//  Square
//
//  Created by Mikhail Shulepov on 15/07/15.
//
//

#ifndef __Square__UIProgressHUD__
#define __Square__UIProgressHUD__

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

NS_CC_BEGIN
class Sprite;
class Label;
class EventListenerTouchOneByOne;
NS_CC_END

class ProgressHUD: public cocos2d::Node {
public:
    class ContentNode;
    enum class PositionGravity {
        Top, Center, Bottom
    };
    
    static void setDefaultCcb(const std::string &ccbFilePath);
    static ProgressHUD *create(const std::string &ccbFilePath);
    static ProgressHUD *create();
    virtual ~ProgressHUD();
    
    void setBlackout();
    void setSwallowTouches(bool swallow);
    void setDissmissOnTouch(bool dismiss);
    
    void setPositionGravity(PositionGravity gravity);
    void setExactPosition(float yPosition);
    
    // Needed in rare cases when progress hud should be shown in new scene, while this scene not active yet
    // ProgressHUD will be added to rootNode
    void setRootNode(cocos2d::Node *node) {
        rootNode = node;
    }
    
    //it will be automatically removed from screen after delay
    void showSuccessWithStatus(const std::string &status);
    void showFailureWithStatus(const std::string &status);
    void showInfoWithStatus(const std::string &status);
    void showLoadingWithStatus(const std::string &status);
    
    void dismiss();
    void dismissAfter(float delay);
    
private:
    ProgressHUD(ContentNode *contentNode);
    void presentAndRunAppearAnimations();
    void runDisappearAnimationsAndRemove();
    void scheduleDismiss();
    
    static std::string defaultCcb;
    
    cocos2d::EventListenerTouchOneByOne *touchHandler;
    cocos2d::Node *blackoutNode;
    cocos2d::Node *rootNode;
    ContentNode *contentNode;
    
    bool dismissScheduled = false;
    bool dismissOnTouch = false;
    bool blackout = false;
};

#endif /* defined(__Square__UIProgressHUD__) */
