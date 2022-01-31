//
//  ShareManager.h
//  CocosFrameworksBinding
//
//  Created by Mikhail Shulepov on 10/06/15.
//  Copyright (c) 2015 Planemo. All rights reserved.
//

#ifndef __CocosFrameworksBinding__ShareManager__
#define __CocosFrameworksBinding__ShareManager__

#include <string>
#include "2d/CCNode.h"
#include "math/CCGeometry.h"

class ShareManager {
public:
    ShareManager() {}
    ~ShareManager() {}
    
    using Completion = std::function<void(bool success, const std::string &sharedTo)>;
    
    ShareManager *setImage(const std::string &imagePath) {
        this->imagePath = imagePath;
        return this;
    }
    
    ShareManager *setText(const std::string &text) {
        this->text = text;
        return this;
    }
    
    ShareManager *setUrl(const std::string &url) {
        this->url = url;
        return this;
    }
    
    // Required for iOS on iPad
    ShareManager *setSourceRect(const cocos2d::Rect &rect) {
        this->glRect = rect;
        return this;
    }
    
    ShareManager *setSourceNode(cocos2d::Node *node) {
        auto sourceRect = node->getBoundingBox();
        auto min = node->getParent()->convertToWorldSpace(cocos2d::Vec2(sourceRect.getMinX(), sourceRect.getMinY()));
        auto max = node->getParent()->convertToWorldSpace(cocos2d::Vec2(sourceRect.getMaxX(), sourceRect.getMaxY()));
        sourceRect.origin = min;
        sourceRect.size = cocos2d::Size(max.x - min.x, max.y - min.y);
        return setSourceRect(sourceRect);
    }
    
    void doShare(Completion completion = nullptr);
    
private:
    std::string imagePath;
    std::string text;
    std::string url;
    
    cocos2d::Rect glRect;
};

#endif /* defined(__CocosFrameworksBinding__ShareManager__) */
