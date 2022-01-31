//
//  ChipmunkDebugNode.h
//  Pulp
//
//  Created by Mikhail Shulepov on 20/07/15.
//
//

#pragma once

#include "extensions/ExtensionMacros.h"
#include "2d/CCDrawNode.h"
#include "extensions/ExtensionExport.h"

struct cpSpace;

class ChipmunkDebugNode : public cocos2d::DrawNode {
    
public:
    /** Create a debug node for a regular Chipmunk space. */
    static ChipmunkDebugNode* create(cpSpace *space);
    /**
     * @js ctor
     */
    ChipmunkDebugNode();
    /**
     * @js NA
     */
    virtual ~ChipmunkDebugNode();
    
    
    cpSpace* getSpace() const;
    void setSpace(cpSpace *space);
    
    // Overrides
    virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
    
protected:
    cpSpace *_spacePtr;
    
};