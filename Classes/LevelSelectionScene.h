//
//  LevelSelectionScene.h
//  Pulp
//
//  Created by Mikhail Shulepov on 23/07/15.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCBMemberVariableAssigner.h"
#include "cocosbuilder/CCNodeLoaderListener.h"
#include "UI/UICollectionView.h"

NS_CC_BEGIN
class Scene;
class Label;
NS_CC_END

class CCBButton;

class LevelSelectionScene
: public cocos2d::Node
, public cocosbuilder::CCBMemberVariableAssigner
, public cocosbuilder::NodeLoaderListener
, public cocos2d::CollectionView::DataSource
, public cocos2d::CollectionView::Delegate {    
public:
    static cocos2d::Scene *createScene();
    virtual ~LevelSelectionScene();
    
    virtual bool onAssignCCBMemberVariable(cocos2d::Ref *target, const std::string &memberVariableName, cocos2d::Node *node) override;
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    
    virtual void onEnter() override;
    
    void didSelectItem(cocos2d::CollectionView *view, cocos2d::CollectionView::ItemNumber item) override;
    virtual cocos2d::CollectionViewItem *viewForItem(cocos2d::CollectionView *view, cocos2d::CollectionView::ItemNumber item) override;
    virtual size_t numberOfItems(cocos2d::CollectionView *view) override;
    
    void close();
    
private:
    friend class LevelSelectionSceneLoader;
    CREATE_FUNC(LevelSelectionScene);
    LevelSelectionScene();
    
    std::string getProgressTitle() const;
    
private:
    static const std::string LEVEL_ITEM_REUSE_ID;
    cocos2d::CollectionView *collectionView;
};

class LevelSelectionSceneLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~LevelSelectionSceneLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(LevelSelectionSceneLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(LevelSelectionScene);
};