//
//  TexturedBackground.cpp
//  AAHedgehog
//
//  Created by Mikhail Shulepov on 14.12.15.
//
//

#include "2d/CCSprite.h"
#include "2d/CCLayer.h"

#include "Resources/Resources.h"

#include "TexturedBackground.h"
USING_NS_CC;

void TexturedBackground::onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) {
    /*LayerColor *background = LayerColor::create(Color4B::WHITE, getContentSize().width, getContentSize().height);
    background->setAnchorPoint(Point::ZERO);
    addChild(background);*/
    
    /*const Vec2 dir = Vec2(-getContentSize().width, getContentSize().height).getNormalized();
    LayerGradient *layer = LayerGradient::create(Color4B(22, 24, 47, 255), Color4B(41, 48, 107, 255), dir);
    //LayerColor *layer = LayerColor::create(Color4B(201, 252, 252, 255), getContentSize().width, getContentSize().height);
    layer->setContentSize(getContentSize());
    layer->setAnchorPoint(Point::ZERO);
    addChild(layer);*/
    
    /*
    Sprite *backgroundTex = Sprite::create(R::Images::Unscaled::BACKGROUND_IMG);
    Texture2D::TexParams params;
    params.minFilter = GL_LINEAR;
    params.magFilter = GL_LINEAR;
    params.wrapS = GL_REPEAT;
    params.wrapT = GL_REPEAT;
    backgroundTex->getTexture()->setTexParameters(params);
    const Size size(backgroundTex->getContentSize().width, getContentSize().height);
    backgroundTex->setTextureRect(cocos2d::Rect(Vec2::ZERO, size));
    backgroundTex->setScaleX(getContentSize().width / size.width);
    backgroundTex->setAnchorPoint(Vec2::ZERO);
    node->addChild(backgroundTex);*/
}

void TexturedBackground::offsetBy(float offset, float duration) {
    
}
