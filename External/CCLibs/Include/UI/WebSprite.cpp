//
//  WebSprite.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 30/07/15.
//
//

#include "WebSprite.h"
#include "Utils/FileUrlCache.h"

USING_NS_CC;

WebSprite *WebSprite::create(const std::string &imageUrl, const Size &preferredSize) {
    WebSprite *ret = new WebSprite;
    ret->initWithImageUrl(imageUrl, preferredSize);
    ret->autorelease();
    return ret;
}

bool WebSprite::initWithImageUrl(const std::string &pImageUrl, const Size &pPreferredSize) {
    setImageUrl(pImageUrl, pPreferredSize);
    return true;
}

void WebSprite::setImageUrl(const std::string &pImageUrl, const cocos2d::Size &pPreferredSize) {
    imageUrl = pImageUrl;
    preferredSize = pPreferredSize;
    
    auto thisAliveMarker = aliveMarker;
    FileUrlCache::getInstance()->cacheDataAtUrl(imageUrl, [this, pImageUrl, thisAliveMarker](const FileUrlCache::CacheResult &result) {
        if (*thisAliveMarker == true) {
            if (result.success && this->imageUrl == pImageUrl && !result.filePath.empty()) {
                this->setTexture(result.filePath);
                this->updateSize();
            }
        }
    });

}

WebSprite::WebSprite() {
    aliveMarker = std::make_shared<bool>(true);
}

WebSprite::~WebSprite() {
    *aliveMarker = false;
}

void WebSprite::updateSize() {
    const Size contentSize = getContentSize();
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    
    if (preferredSize.width > FLT_EPSILON) {
        scaleX = preferredSize.width / contentSize.width;
    }
    if (preferredSize.height > FLT_EPSILON) {
        scaleY = preferredSize.height / contentSize.height;
    }
    const float scale = std::min(scaleX, scaleY);
    setScale(scale);
}