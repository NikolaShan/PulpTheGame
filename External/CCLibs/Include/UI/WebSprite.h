//
//  WebSprite.h
//  Pulp
//
//  Created by Mikhail Shulepov on 30/07/15.
//
//

#pragma once

#include "2d/CCSprite.h"
#include "cocosbuilder/CCSpriteLoader.h"

class WebSprite: public cocos2d::Sprite {
public:
    CREATE_FUNC(WebSprite);
    static WebSprite *create(const std::string &imageUrl, const cocos2d::Size &preferredSize);
    bool initWithImageUrl(const std::string &imageUrl, const cocos2d::Size &preferredSize);
    
    void setImageUrl(const std::string &imageUrl, const cocos2d::Size &preferredSize);
    
    WebSprite();
    virtual ~WebSprite();
    
private:
    void updateSize();
    
private:
    std::string imageUrl;
    cocos2d::Size preferredSize;
    std::shared_ptr<bool> aliveMarker;
};

class WebSpriteLoader : public cocosbuilder::SpriteLoader {
public:
    virtual ~WebSpriteLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(WebSpriteLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(WebSprite);
};
