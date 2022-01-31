//
//  ChipmunkDebugNode.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 20/07/15.
//
//

#include "ChipmunkDebugNode.h"

#include "chipmunk/chipmunk.h"
#include "chipmunk/chipmunk_private.h"

#include "base/ccTypes.h"
#include "math/CCGeometry.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>


USING_NS_CC;

static Color4F ColorForBody(cpBody *body) {
    if (cpBodyGetType(body) == CP_BODY_TYPE_KINEMATIC || cpBodyIsSleeping(body)) {
        return Color4F(0.5f, 0.5f, 0.5f ,0.5f);
    } else if (body->sleeping.idleTime > body->space->sleepTimeThreshold) {
        return Color4F(0.33f, 0.33f, 0.33f, 0.5f);
    } else {
        return Color4F(1.0f, 0.0f, 0.0f, 0.5f);
    }
}

static Vec2 cpVert2Point(const cpVect &vert) {
    return Vec2(vert.x, vert.y);
}

static Vec2* cpVertArray2ccpArrayN(const cpVect* cpVertArray, unsigned int count) {
    if (count == 0) return nullptr;
    Vec2* pPoints = new (std::nothrow) Vec2[count];
    
    for (unsigned int i = 0; i < count; ++i) {
        pPoints[i].x = cpVertArray[i].x;
        pPoints[i].y = cpVertArray[i].y;
    }
    return pPoints;
}

static void DrawShape(cpShape *shape, DrawNode *renderer) {
    cpBody *body = shape->body;
    Color4F color = ColorForBody(body);
    
    switch (shape->klass->type) {
        case CP_CIRCLE_SHAPE:  {
            cpCircleShape *circle = (cpCircleShape *)shape;
            cpVect center = circle->tc;//cpBodyLocalToWorld(cpShapeGetBody(shape), circle->tc);
            cpFloat radius = circle->r;
            renderer->drawDot(cpVert2Point(center), cpfmax(radius, 1.0), color);
//            renderer->drawSegment(cpVert2Point(center), cpVert2Point(cpvadd(center, cpvmult(body->rot, radius))), 1.0, color);
        }
            break;
        case CP_SEGMENT_SHAPE: {
            cpSegmentShape *seg = (cpSegmentShape *)shape;
            renderer->drawSegment(cpVert2Point(seg->ta), cpVert2Point(seg->tb), cpfmax(seg->r, 2.0), color);
        }
            break;
        case CP_POLY_SHAPE: {
            cpPolyShape *poly = (cpPolyShape *)shape;
            Color4F line = color;
            line.a = cpflerp(color.a, 1.0, 0.5);
//            Vec2* pPoints = cpVertArray2ccpArrayN(poly->tVerts, poly->numVerts);
//            renderer->drawPolygon(pPoints, poly->numVerts, color, 1.0, line);
            //CC_SAFE_DELETE_ARRAY(pPoints);
        }
            break;
        default:
            cpAssertHard(false, "Bad assertion in DrawShape()");
    }
}

static Color4F CONSTRAINT_COLOR(0, 1, 0, 0.5);

static void DrawConstraint(cpConstraint *constraint, DrawNode *renderer)
{
    /*cpBody *body_a = constraint->a;
    cpBody *body_b = constraint->b;
    
    const cpConstraintClass *klass = constraint->CP_PRIVATE(klass);
    if (klass == cpPinJointGetClass())
    {
        cpPinJoint *joint = (cpPinJoint *)constraint;
        
        cpVect a = cpBodyLocal2World(body_a, joint->anchr1);
        cpVect b = cpBodyLocal2World(body_b, joint->anchr2);
        
        renderer->drawDot(cpVert2Point(a), 3.0, CONSTRAINT_COLOR);
        renderer->drawDot(cpVert2Point(b), 3.0, CONSTRAINT_COLOR);
        renderer->drawSegment(cpVert2Point(a), cpVert2Point(b), 1.0, CONSTRAINT_COLOR);
    }
    else if (klass == cpSlideJointGetClass())
    {
        cpSlideJoint *joint = (cpSlideJoint *)constraint;
        
        cpVect a = cpBodyLocal2World(body_a, joint->anchr1);
        cpVect b = cpBodyLocal2World(body_b, joint->anchr2);
        
        renderer->drawDot(cpVert2Point(a), 3.0, CONSTRAINT_COLOR);
        renderer->drawDot(cpVert2Point(b), 3.0, CONSTRAINT_COLOR);
        renderer->drawSegment(cpVert2Point(a), cpVert2Point(b), 1.0, CONSTRAINT_COLOR);
    }
    else if (klass == cpPivotJointGetClass())
    {
        cpPivotJoint *joint = (cpPivotJoint *)constraint;
        
        cpVect a = cpBodyLocal2World(body_a, joint->anchr1);
        cpVect b = cpBodyLocal2World(body_b, joint->anchr2);
        
        renderer->drawDot(cpVert2Point(a), 3.0, CONSTRAINT_COLOR);
        renderer->drawDot(cpVert2Point(b), 3.0, CONSTRAINT_COLOR);
    }
    else if (klass == cpGrooveJointGetClass())
    {
        cpGrooveJoint *joint = (cpGrooveJoint *)constraint;
        
        cpVect a = cpBodyLocal2World(body_a, joint->grv_a);
        cpVect b = cpBodyLocal2World(body_a, joint->grv_b);
        cpVect c = cpBodyLocal2World(body_b, joint->anchr2);
        
        renderer->drawDot(cpVert2Point(c), 3.0, CONSTRAINT_COLOR);
        renderer->drawSegment(cpVert2Point(a), cpVert2Point(b), 1.0, CONSTRAINT_COLOR);
    }
    else if (klass == cpDampedSpringGetClass())
    {
        // TODO: uninplemented
    }
    else
    {
        //		printf("Cannot draw constraint\n");
    }*/
}


// implementation of PhysicsDebugNode

void ChipmunkDebugNode::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) {
    if (! _spacePtr) {
        return;
    }
    // clear the shapes information before draw current shapes.
    DrawNode::clear();
    
    cpSpaceEachShape(_spacePtr, (cpSpaceShapeIteratorFunc)DrawShape, this);
    cpSpaceEachConstraint(_spacePtr, (cpSpaceConstraintIteratorFunc)DrawConstraint, this);
    
    DrawNode::draw(renderer, transform, flags);
}

ChipmunkDebugNode::ChipmunkDebugNode()
: _spacePtr(nullptr)
{}

ChipmunkDebugNode* ChipmunkDebugNode::create(cpSpace *space) {
    ChipmunkDebugNode *node = new (std::nothrow) ChipmunkDebugNode();
    if (node) {
        node->init();
        node->_spacePtr = space;
        node->autorelease();
    } else {
        CC_SAFE_DELETE(node);
    }
    
    return node;
}

ChipmunkDebugNode::~ChipmunkDebugNode() {
}

cpSpace* ChipmunkDebugNode::getSpace() const {
    return _spacePtr;
}

void ChipmunkDebugNode::setSpace(cpSpace *space) {
    _spacePtr = space;
}