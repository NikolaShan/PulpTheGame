//
//  SegmentMaker.cpp
//  Pulp
//
//  Created by Mikhail Shulepov on 17/07/15.
//
//

#include "SegmentMaker.h"

#include "Resources/Resources.h"

#include "base/CCDirector.h"
#include "2d/CCRenderTexture.h"
#include "2d/CCClippingNode.h"
#include "2d/CCAutoPolygon.h"
#include "2d/CCLayer.h"
#include "2d/CCNode.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/CCGLStateCache.h"
#include "renderer/CCGLProgram.h"
#include "renderer/CCRenderer.h"
#include "Utils/StringUtils.h"

USING_NS_CC;

class CircleNode: public cocos2d::LayerColor {
private:
    
public:
    static CircleNode *create(float innerRadius, float outerRadius) {
        auto ret = new CircleNode();
        ret->init(innerRadius, outerRadius);
        ret->autorelease();
        return ret;
    }
    
    bool init(float innerRadius, float outerRadius) {
        const float size = outerRadius * 2 + 4;
        
        LayerColor::initWithColor(Color4B::GREEN, size, size);
        setGLProgram(getCircleShader(innerRadius, outerRadius, size));
        
        _squareColors[0] = Color4F(0, 0, 0, 1);
        _squareColors[1] = Color4F(1, 0, 0, 1);
        _squareColors[2] = Color4F(0, 1, 0, 1);
        _squareColors[3] = Color4F(1, 1, 0, 1);
        
        return true;
    }
    
    void onDraw(const Mat4& transform, uint32_t flags) {
        getGLProgram()->use();
        getGLProgram()->setUniformsForBuiltins(transform);
        
        GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_COLOR);
        
        //
        // Attributes
        //
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, _noMVPVertices);
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_FLOAT, GL_FALSE, 0, _squareColors);
        
        GL::blendFunc( _blendFunc.src, _blendFunc.dst );
        
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        
        CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1,4);
    }
    
    GLProgram *getCircleShader(float innerRadius, float outerRadius, float size) {
        GLProgram *p = new GLProgram();
        p->initWithFilenames(R::Shaders::SHADER_CIRCLE_VERT, R::Shaders::SHADER_SHADOWED_CIRCLE_FRAG);
        
        p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::VERTEX_ATTRIB_POSITION);
        p->bindAttribLocation(GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::VERTEX_ATTRIB_COLOR);
        
        p->link();
        
        p->updateUniforms();
        
        GLuint outerRadiusLocation = p->getUniformLocation("u_outerRadius");
        p->setUniformLocationWith1f(outerRadiusLocation, outerRadius / size);
        
        GLuint innerRadiusLocation = p->getUniformLocation("u_innerRadius");
        p->setUniformLocationWith1f(innerRadiusLocation, innerRadius / size);
        
        GLuint shadowSizeLocation = p->getUniformLocation("u_shadowSize");
        p->setUniformLocationWith1f(shadowSizeLocation, 4.5f / size);
        
        GLuint blurLocation = p->getUniformLocation("u_blur");
        p->setUniformLocationWith1f(blurLocation, 1.1f / size);
        
        p->autorelease();
        
        return p;
    }
};



LayerColor *CircleMaker::makeCircleWithRadius(float innerRadius, float outerRadius) {
    LayerColor *circle = CircleNode::create(innerRadius, outerRadius);
    circle->setAnchorPoint(Vec2(0.5f, 0.5f));
    circle->setIgnoreAnchorPointForPosition(false);
    return circle;
}

SegmentMaker::~SegmentMaker() {
    CC_SAFE_RELEASE(targetSprite);
}

void SegmentMaker::setRadius(double near, double far) {
    if (std::abs(near - radiusNear) > FLT_EPSILON || std::abs(far - radiusFar) > FLT_EPSILON) {
        this->radiusFar = far;
        this->radiusNear = near;
        CC_SAFE_RELEASE_NULL(targetSprite);
    }
}

void SegmentMaker::prepareSprite() {
    const float fullSize = (radiusFar + 25) * 2;
    
    LayerColor *circleOuter = CircleMaker::makeCircleWithRadius(radiusNear, radiusFar);
    circleOuter->setPosition(fullSize / 2, fullSize / 2);
    
    RenderTexture *renderTexture = RenderTexture::create(fullSize, fullSize, Texture2D::PixelFormat::RGBA8888);//, GL_DEPTH24_STENCIL8);
    
    CC_SAFE_RELEASE(targetSprite);
    targetSprite = renderTexture->getSprite();
    targetSprite->getTexture()->setAntiAliasTexParameters();
    targetSprite->setOpacityModifyRGB(true);

    renderTexture->beginWithClear(0.0f, 0.0f, 0.0f, 0.0f);
    circleOuter->visit();
    renderTexture->end();
    
    targetSprite->retain();
}

Sprite *SegmentMaker::makeSprite() {
    if (!targetSprite) {
        prepareSprite();
    }
    Sprite *ret;
    if (angle >= M_PI * 2 - FLT_EPSILON) {
        ret = Sprite::createWithTexture(targetSprite->getTexture());
    } else {
        ret = makeSpriteAsCircle();
    }
    ret->setScale(targetSprite->getScale());
    ret->setBlendFunc(targetSprite->getBlendFunc());
    ret->setOpacityModifyRGB(targetSprite->isOpacityModifyRGB());
    return ret;
}

Sprite *SegmentMaker::makeSpriteAsSegment() {
    const float fullSize = targetSprite->getContentSize().width;
    
    std::vector<Vec2> outerPoints;
    
    if (angle <= M_PI / 2) {
        const float xOffset = fabsf( tanf(angle / 2) * fullSize / 2 );
        const float topLeft = fullSize / 2 - xOffset;
        const float topRight = fullSize / 2 + xOffset;
        
        outerPoints.emplace_back(topLeft, fullSize);
        outerPoints.emplace_back(topRight, fullSize);
        
    } else if (angle <= 1.5 * M_PI) {
        float yOffset;
        if (angle < M_PI) {
            yOffset = fullSize / 2 + fabsf(tanf(M_PI / 2 - angle / 2) * fullSize / 2);
        } else {
            yOffset = fullSize / 2 - fabsf(tanf(angle / 2 - M_PI / 2) * fullSize / 2);
        }
        
        outerPoints.emplace_back(0, yOffset); //lower left
        outerPoints.emplace_back(0, fullSize); //left top
        outerPoints.emplace_back(fullSize, fullSize); //right top
        outerPoints.emplace_back(fullSize, yOffset); //lower right
        
    } else {
        const float xOffset = fabsf( tanf(M_PI - angle / 2) * fullSize / 2);
        const float xLeft = fullSize / 2 - xOffset;
        const float xRight = fullSize / 2 + xOffset;
        
        outerPoints.emplace_back(xLeft, 0); //left
        outerPoints.emplace_back(0, 0); //left bottom
        outerPoints.emplace_back(0, fullSize); //left top
        outerPoints.emplace_back(fullSize, fullSize); //right top
        outerPoints.emplace_back(fullSize, 0); //right bottom
        outerPoints.emplace_back(xRight, 0); //right
    }
    
    const size_t trianglesCount = outerPoints.size() - 1;
    
    PolygonInfo polygonInfo;
    polygonInfo.filename = "generated_texure_" + ::StringUtils::toString((int)radiusFar);
    polygonInfo.triangles.vertCount = static_cast<int>(outerPoints.size()) + 1;
    polygonInfo.triangles.indexCount = static_cast<int>(trianglesCount * 3);
    
    const Vec2 centrePoint(fullSize / 2, fullSize / 2);
    
    // fill vertices - 0 - center, 1... - outer
    std::vector<Vec2> points = outerPoints;
    points.insert(points.begin(), centrePoint);
    V3F_C4B_T2F *verts = new V3F_C4B_T2F[points.size() + 1];
    for (size_t i = 0; i < points.size(); ++i) {
        const Vec2 point = points[i];
        verts[i].vertices = Vec3(point.x, point.y, 0);
        verts[i].colors = Color4B::WHITE;
        verts[i].texCoords = Tex2F(point.x / fullSize, point.y / fullSize);
    }
    polygonInfo.triangles.verts = verts;
    
    // fill indices
    unsigned short *indices = new unsigned short[polygonInfo.triangles.indexCount];
    unsigned short start = 1;
    for (size_t triangle = 0; triangle < trianglesCount; ++triangle) {
        indices[triangle * 3] = 0; //centre
        indices[triangle * 3 + 1] = start;
        indices[triangle * 3 + 2] = start + 1;
        start++;
    }
    polygonInfo.triangles.indices = indices;
    
    Sprite *ret = Sprite::createWithTexture(targetSprite->getTexture());
    ret->setPolygonInfo(polygonInfo);
    return ret;
}

Sprite *SegmentMaker::makeSpriteAsCircle() {
    const float fullSize = targetSprite->getContentSize().width;
    
    PolygonInfo polygonInfo;
    polygonInfo.filename = "generated_texure_"
        + ::StringUtils::toString((int)radiusNear) + "x" + ::StringUtils::toString((int)radiusFar);
       
    const double startAnglePoint = (-angle / 2 + M_PI / 2 - 0.005f);
    const double endAnglePoint = (angle / 2 + M_PI / 2 + 0.005f);
    
    const double angleDelta = endAnglePoint - startAnglePoint;
    const size_t segmentsCount = ceil( angleDelta / (M_PI / 5) );
    const double angleStep = angleDelta / segmentsCount;
    const double halfTangentAngle = (M_PI - angleStep) / 2;
    
    const double center = fullSize / 2;
    
    std::vector<double> angles;
    angles.push_back(startAnglePoint);
    for (double angle = startAnglePoint + angleStep / 2; angle < endAnglePoint; angle += angleStep) {
        angles.push_back(angle);
    }
    angles.push_back(endAnglePoint);
    
    std::vector<Vec2> outerPoints;
    std::vector<Vec2> innerPoints;
    
    // outer
    {
        const double extendedRadius = radiusFar + 4;
        const double halfWidth = extendedRadius * sin(angleStep * 0.5);
        const double preHeight = extendedRadius * cos(angleStep * 0.5);
        const double postHeight = halfWidth / tan(halfTangentAngle);
        const double outerCircleRadius = preHeight + postHeight;
           
        for (double angle: angles) {
            float x = center + outerCircleRadius * cos(angle);
            float y = center + outerCircleRadius * sin(angle);
           
            x = clampf(x, 0, fullSize);
            y = clampf(y, 0, fullSize);
            
            outerPoints.push_back(Vec2(x, y));
        }
    }
    
    // inner
    {
        const double innerCircleRadius = radiusNear - 2;
        for (double angle: angles) {
            const float x = center + innerCircleRadius * cos(angle - innerAngleOffset);
            const float y = center + innerCircleRadius * sin(angle - innerAngleOffset);
            innerPoints.push_back(Vec2(x, y));
        }
    }
    
    // fill vertices
    polygonInfo.triangles.vertCount = static_cast<int>(outerPoints.size() + innerPoints.size());
    V3F_C4B_T2F *verts = new V3F_C4B_T2F[polygonInfo.triangles.vertCount];
    for (size_t i = 0; i < outerPoints.size(); ++i) {
        size_t p = i * 2;
        
        const Vec2 outer = outerPoints[i];
        verts[p].vertices = Vec3(outer.x, outer.y, 0);
        verts[p].colors = Color4B::WHITE;
        verts[p].texCoords = Tex2F(outer.x / fullSize, outer.y / fullSize);
        
        const Vec2 inner = innerPoints[i];
        verts[p+1].vertices = Vec3(inner.x, inner.y, 0);
        verts[p+1].colors = Color4B::WHITE;
        verts[p+1].texCoords = Tex2F(inner.x / fullSize, inner.y / fullSize);
    }
    polygonInfo.triangles.verts = verts;
    
    // fill indices
    const size_t trianglesCount = outerPoints.size() * 2 - 2;
    polygonInfo.triangles.indexCount = static_cast<int>(trianglesCount * 3);
    unsigned short *indices = new unsigned short[polygonInfo.triangles.indexCount];
    unsigned short start = 0;
    for (size_t triangleNum = 0; triangleNum < trianglesCount; ++triangleNum) {
        const size_t p = triangleNum * 3;
        if (triangleNum % 2 == 0) {
            indices[p] = start;
            indices[p + 1] = start + 1;
            indices[p + 2] = start + 2;
        } else {
            indices[p] = start;
            indices[p + 1] = start + 2;
            indices[p + 2] = start + 1;
        }
        ++start;
    }
    polygonInfo.triangles.indices = indices;
    
    Sprite *ret = Sprite::createWithTexture(targetSprite->getTexture());
    ret->setPolygonInfo(polygonInfo);
    return ret;
}

std::vector<SegmentMaker::ConvexPolygon> SegmentMaker::makePhysicsPolygons() const {
    // Segment should be oriented to the top so add angle PI/2
    // For PI/2 segment startPoint=PI/4, endPoint=3PI/4
    // For 2PI/3 segment startPoint=PI/6, endPoint=5PI/6
    
    if (angle < M_PI) {
        const double startAnglePoint = (-angle / 2 + M_PI / 2);
        const double endAnglePoint = (angle / 2 + M_PI / 2);
        return {makePhysicsPolygon(startAnglePoint, endAnglePoint)};
    } else {
        const double startAnglePoint = (-angle / 2 + M_PI / 2);
        const double endAnglePoint = (angle / 2 + M_PI / 2);
        auto polygonLeft = makePhysicsPolygon(startAnglePoint, M_PI / 2);
        auto polygonRight = makePhysicsPolygon(M_PI / 2, endAnglePoint);
        return {polygonLeft, polygonRight};
    }
}

SegmentMaker::ConvexPolygon SegmentMaker::makePhysicsPolygon(double startAnglePoint, double endAnglePoint) const {
    const double angleDelta = endAnglePoint - startAnglePoint;
    const size_t segmentsCount = ceil( angleDelta / (M_PI / 9) );
    const double angleStep = angleDelta / segmentsCount;
    
    std::vector<Vec2> ret;
    
    // outer
    for (double angle = startAnglePoint; angle <= endAnglePoint + FLT_EPSILON; angle += angleStep) {
        const float x = radiusFar * cos(angle);
        const float y = radiusFar * sin(angle);
        ret.push_back(Vec2(x, y));
    }
    
    // inner
    std::vector<double> innerAngles = { startAnglePoint, endAnglePoint };
    for (double angle: innerAngles) {
        const float x = radiusNear * cos(angle - innerAngleOffset);
        const float y = radiusNear * sin(angle - innerAngleOffset);
        ret.push_back(Vec2(x, y));
    }
       
    return ret;

}