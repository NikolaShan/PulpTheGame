//
//  TexturedBackground.hpp
//  AAHedgehog
//
//  Created by Mikhail Shulepov on 14.12.15.
//
//

#pragma once

#include "2d/CCNode.h"
#include "cocosbuilder/CCNodeLoader.h"
#include "cocosbuilder/CCNodeLoaderListener.h"

class TexturedBackground: public cocos2d::Node, public cocosbuilder::NodeLoaderListener {
public:
    CREATE_FUNC(TexturedBackground);
    
    virtual void onNodeLoaded(cocos2d::Node *node, cocosbuilder::NodeLoader *nodeLoader) override;
    void offsetBy(float offset, float duration);
    
private:
    friend class TexturedBackgroundLoader;
    TexturedBackground() {}
};

class TexturedBackgroundLoader : public cocosbuilder::NodeLoader {
public:
    virtual ~TexturedBackgroundLoader() {};
    CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(TexturedBackgroundLoader, loader);
    
protected:
    CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(TexturedBackground);
};