//
//  ShareBanner.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 15.12.15.
//
//

#include "ShareBanner.h"

#include "cocosbuilder/CocosBuilder.h"
#include "2d/CCRenderTexture.h"
#include "2d/CCLabel.h"
#include "base/CCDirector.h"

#include "Utils/GuiUtils.h"
#include "Utils/StringUtils.h"
#include "Resources/Resources.h"
#include "AppSettings.h"
#include "GameScene.h"
#include "Game/GameManager.h"

USING_NS_CC;
using namespace cocosbuilder;


ShareBanner *ShareBanner::createFromCcb() {
    auto nodeLoaderLibrary = NodeLoaderLibrary::getInstance();
    nodeLoaderLibrary->registerNodeLoader("ShareBanner", ShareBannerLoader::loader());
    auto ccbReader = new CCBReader(nodeLoaderLibrary);
    
    cocos2d::Size size(600, 300);
    size = size / CC_CONTENT_SCALE_FACTOR();
    ShareBanner *node = dynamic_cast<ShareBanner *>(ccbReader->readNodeGraphFromFile(UI_FILE(R::UI::Share::SHARE_BANNER), nullptr, size));
    ccbReader->release();
    
    return node;
}

ShareBanner::ShareBanner() {
    
}

ShareBanner::~ShareBanner() {
    
}
    
bool ShareBanner::onAssignCCBMemberVariable(Ref *target, const std::string &memberVariableName, Node *node) {
    if (memberVariableName == "ScoreLabel") {
        Label *label = dynamic_cast<Label *>(node);
        label->setSystemFontSize(label->getSystemFontSize() / CC_CONTENT_SCALE_FACTOR());
        scoreLabel = label;
        return true;
    }
    if (memberVariableName == "TitleLabel") {
        Label *label = dynamic_cast<Label *>(node);
        label->setSystemFontSize(label->getSystemFontSize() / CC_CONTENT_SCALE_FACTOR());
        titleLabel = label;
        return true;
    }
    if (memberVariableName == "ScreenshotContainer") {
        levelContainer = node;
        return true;
    }
    return false;
}

void ShareBanner::onNodeLoaded(Node *node, NodeLoader *nodeLoader) {
}

void ShareBanner::setupForLevelShare(int level) {
    scoreLabel->setString("Level " + ::StringUtils::toString(level));
    titleLabel->setString("Can you get this far?");

    auto gm = GameManager::getInstance();
    const int currentLevel = gm->getCurrentLevelNumber();
    gm->setCurrentLevel(level);
    setupLevelInside(levelContainer, false);
    
    gm->setCurrentLevel(currentLevel);
}

void ShareBanner::setupForScoreShare(int score) {
    scoreLabel->setString("Score " + ::StringUtils::toString(score));
    titleLabel->setString("Can you get this far?");
    
    auto gm = GameManager::getInstance();
    const int currentLevel = gm->getCurrentLevelNumber();
    gm->setCurrentLevel(1 + std::rand() % gm->getLevelsCount());
    setupLevelInside(levelContainer, true);
    
    gm->setCurrentLevel(currentLevel);
}

void ShareBanner::setupLevelInside(Node *node, bool endless) {
    const float scale = 320.0f / node->getContentSize().width;
    cocos2d::Size size(320.0f, node->getContentSize().height * scale);
    GameScene *gk = GameScene::createSceneForScreenshot(size, scale);
    if (endless) {
        gk->getLevelNumberLabel()->setString("∞");
    }
    //gk shouldn't be released before CCBReader...
    scene = gk;
    
    auto renderTex = RenderTexture::create(size.width, size.height, Texture2D::PixelFormat::RGBA8888);
    renderTex->begin();
    gk->visit();
    renderTex->end();
    
    renderTex->getSprite()->getTexture()->setAntiAliasTexParameters();
    renderTex->getSprite()->setAnchorPoint(Vec2::ZERO);
    renderTex->getSprite()->setScale(1.0f / scale);
    renderTex->getSprite()->removeFromParent();
    node->addChild(renderTex->getSprite());
}

void ShareBanner::takeScreenshot(std::function<void(const std::string &filePath)> result) {
    retain();
    auto rt = RenderTexture::create(getContentSize().width, getContentSize().height, Texture2D::PixelFormat::RGBA8888);
    rt->getSprite()->getTexture()->setAntiAliasTexParameters();
    rt->begin();
    visit();
    rt->end();
    
    rt->saveToFile(AppSettings::ScreenshotImage, Image::Format::PNG, false, [this, result](RenderTexture *tex, const std::string &path) {
        result(path);
        release();
    });
}
