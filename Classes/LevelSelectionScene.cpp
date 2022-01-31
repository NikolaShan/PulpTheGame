//
//  LevelSelectionScene.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 23/07/15.
//
//

#include "LevelSelectionScene.h"

#include "2d/CCLabel.h"
#include "2d/CCScene.h"
#include "2d/CCTransition.h"
#include "base/CCDirector.h"
#include "cocosbuilder/CocosBuilder.h"

#include "Utils/GuiUtils.h"
#include "Utils/StringUtils.h"
#include "Utils/AppUtils.h"
#include "UI/CollectionViewCCBItem.h"
#include "UI/CCBButton.h"
#include "Resources/Resources.h"
#include "AnalyticsDeprecated.h"
#include "AppSettings.h"

#include "Game/GameManager.h"
#include "GameScene.h"
#include "MainMenuScene.h"

USING_NS_CC;
using namespace cocosbuilder;

const std::string LevelSelectionScene::LEVEL_ITEM_REUSE_ID = "LevelItem";

Scene *LevelSelectionScene::createScene() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("LevelSelectionScene", LevelSelectionSceneLoader::loader());
    nodeLoaderLibrary->registerNodeLoader("CollectionView", CollectionViewLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    Node *rootNode = ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::LevelSelection::SCENE));
    ccbReader->release();
    
    auto scene = Scene::create();
    scene->addChild(rootNode);
    return scene;
}

LevelSelectionScene::LevelSelectionScene(): collectionView(nullptr) {
    GuiUtils::setupEscapePressHandler(this, [this]() {
        close();
    });
}

LevelSelectionScene::~LevelSelectionScene() {
    CC_SAFE_RELEASE(collectionView);
}
    
bool LevelSelectionScene::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    if (AppUtils::isIPhoneX() && memberVariableName == "TitleLabel") {
        node->setVisible(false);
        return true;
    }
    ASSIGN_VARIABLE("CollectionView", collectionView, CollectionView);
    SETUP_LABEL("TitleLabel", LocalizedString(R::Strings::Levels::TITLE));
    SETUP_LABEL("ProgressLabel", getProgressTitle());
    SETUP_CUSTOM_CCBBUTTON("BackButton", "", close);
    return false;
}

void LevelSelectionScene::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
    const cocos2d::Size defaultSize(50.0f, 50.0f);
    auto itemTemplate = CollectionViewCCBItem::createFromCcb(UI_FILE(R::UI::LevelSelection::LEVEL_ITEM), defaultSize);
    collectionView->setReuseTemplateItem(itemTemplate, LEVEL_ITEM_REUSE_ID);
    collectionView->setDefaultItemSize(defaultSize);
    collectionView->setInterItemSpacing(cocos2d::Size(6, 10));
    collectionView->setContentMargin(Margin(20, 6, 6, 100));
    collectionView->setDataSource(this);
    collectionView->setDelegate(this);
    collectionView->reloadData();
    
    const cocos2d::Size contentSize = collectionView->getContentSize();
    const cocos2d::Size viewSize = collectionView->getViewSize();
    if (contentSize.height > viewSize.height) {
        const int currentLevel = GameManager::getInstance()->getCurrentLevelNumber();
        const cocos2d::Rect itemBounds = collectionView->getItemPosition(currentLevel - 1);
        const float itemMaxY = itemBounds.getMaxY();

        float contentOffset = std::max(itemMaxY - (viewSize.height + itemBounds.size.height) * 0.5f, 0.0f);
        contentOffset = std::min(contentOffset, contentSize.height - viewSize.height);
        collectionView->setContentOffset(Vec2(0.0f, -contentOffset));
    }
}

void LevelSelectionScene::onEnter() {
    Node::onEnter();
}

std::string LevelSelectionScene::getProgressTitle() const {
    auto gm = GameManager::getInstance();
    const int completedCount = gm->getCurrentLevelNumber();
    const int totalCount = gm->getLevelsCount();
    return FormattedLocalizedString(R::Strings::Levels::OUT_OF, completedCount, totalCount);
}

void LevelSelectionScene::didSelectItem(CollectionView *view, CollectionView::ItemNumber item) {
    GameManager::getInstance()->setCurrentLevel(static_cast<int>(item + 1));
    auto gameScene = GameScene::createScene();
    const Color3B transitionColor =AppSettings::getInstance()->isNightModeEnabled() ? Color3B::BLACK : Color3B::WHITE;
    auto transition = TransitionFade::create(0.3, gameScene, transitionColor);
    Director::getInstance()->replaceScene(transition);
}

CollectionViewItem *LevelSelectionScene::viewForItem(CollectionView *view, CollectionView::ItemNumber item) {
    CollectionViewCCBItem *ccbItem = dynamic_cast<CollectionViewCCBItem *>(view->getReusableItem(LEVEL_ITEM_REUSE_ID));
    Label *title = dynamic_cast<Label *>(ccbItem->getNodeNamed("LevelNumberLabel"));
    title->setString(::StringUtils::toString(item + 1));
    return ccbItem;
}

size_t LevelSelectionScene::numberOfItems(cocos2d::CollectionView *view) {
    auto gameManager = GameManager::getInstance();
#if COCOS2D_DEBUG == 1
#warning All levels opened
    return gameManager->getLevelsCount();
#else
    return std::min(gameManager->getCompletedLevelsCount() + 1, gameManager->getLevelsCount());
#endif
}

void LevelSelectionScene::close() {
    auto menuScene = MainMenuScene::createScene(MenuStyle::MAIN_MENU);
    const Color3B transitionColor =AppSettings::getInstance()->isNightModeEnabled() ? Color3B::BLACK : Color3B::WHITE;
    auto transition = TransitionFade::create(0.3, menuScene, transitionColor);
    Director::getInstance()->replaceScene(transition);
}
