/* Copyright (c) 2012 Scott Lembcke and Howling Moon Software
 * Copyright (c) 2012 cocos2d-x.org
 * Copyright (c) 2013-2016 Chukong Technologies Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ccHeader.h"
#include "2d/CCDrawNode.h"
#include "base/CCEventType.h"
#include "renderer/Renderer.h"
#include "base/CCDirector.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventDispatcher.h"
#include "2d/CCActionCatmullRom.h" //for PointArray, awful

NS_CC_BEGIN

// Vec2 == CGPoint in 32-bits, but not in 64-bits (OS X)
// that's why the "v2f" functions are needed
static Vec2 v2fzero(0.0f,0.0f);

static inline Vec2 v2f(float x, float y)
{
    Vec2 ret(x, y);
    return ret;
}

static inline Vec2 v2fadd(const Vec2 &v0, const Vec2 &v1)
{
    return v2f(v0.x+v1.x, v0.y+v1.y);
}

static inline Vec2 v2fsub(const Vec2 &v0, const Vec2 &v1)
{
    return v2f(v0.x-v1.x, v0.y-v1.y);
}

static inline Vec2 v2fmult(const Vec2 &v, float s)
{
    return v2f(v.x * s, v.y * s);
}

static inline Vec2 v2fperp(const Vec2 &p0)
{
    return v2f(-p0.y, p0.x);
}

static inline Vec2 v2fneg(const Vec2 &p0)
{
    return v2f(-p0.x, - p0.y);
}

static inline float v2fdot(const Vec2 &p0, const Vec2 &p1)
{
    return  p0.x * p1.x + p0.y * p1.y;
}

static inline Vec2 v2fforangle(float _a_)
{
    return v2f(cosf(_a_), sinf(_a_));
}

static inline Vec2 v2fnormalize(const Vec2 &p)
{
    Vec2 r(p.x, p.y);
    r.normalize();
    return v2f(r.x, r.y);
}

static inline Vec2 __v2f(const Vec2 &v)
{
//#ifdef __LP64__
    return v2f(v.x, v.y);
// #else
//     return * ((Vec2*) &v);
// #endif
}

static inline Tex2F __t(const Vec2 &v)
{
    return *(Tex2F*)&v;
}

// implementation of DrawNode

DrawNode::DrawNode(int32_t lineWidth)
    : blendFunc_(BlendFunc::ALPHA_NON_PREMULTIPLIED)
{
    
}

void DrawNode::pushVertex(const Vec2& pos, const Vec4& color, const Vec2& coord)
{
    posColors_.push_back({ {pos.x, pos.y, 0, 1}, color });
    vertices_.push_back({ 0, 0, 0, 0, 0, coord.x, coord.y });
}

const BlendFunc& DrawNode::getBlendFunc() const
{
    return blendFunc_;
}

void DrawNode::setBlendFunc(const BlendFunc &var)
{
    blendFunc_ = var;
}

void DrawNode::setDepthWrite(bool var)
{
    flags_.setFlag(DrawNode::DepthWrite, var);
}

uint64_t DrawNode::getRenderState() const
{
    return renderState_;
}

const std::vector<DrawVertex>& DrawNode::getVertices() const
{
    return vertices_;
}

const std::vector<uint16_t>& DrawNode::getIndices() const
{
    return indices_;
}

void DrawNode::draw(IRenderer *renderer, const Mat4 &transform, uint32_t flags)
{
    if (vertices_.empty())
        return;
    if (flags_.isOn(DrawNode::VertexColorDirty))
    {
        flags_.setOff(DrawNode::VertexColorDirty);
        Vec4 ucolor = Color4F(_realColor, (float)_realOpacity / 255.0f).toVec4();
        for (size_t i = 0; i < posColors_.size(); ++i)
        {
            const Vec4& acolor = posColors_[i].color;
            float alpha = acolor.w * ucolor.w;
            Vec4 color{ 0, 0, 0, alpha };
            bx::vec3Mul(color, acolor, ucolor);
            vertices_[i].abgr = Color4B(Color4F(color.x, color.y, color.z, color.w)).toABGR();
        }
    }

    if (flags_.isOn(DrawNode::VertexPosDirty))
    {
        flags_.setOff(DrawNode::VertexPosDirty);
        Mat4 trans;
        bx::mtxMul(trans, transform, SharedDirector.getViewProjection());
        for (size_t i = 0; i < vertices_.size(); ++i)
        {
            bx::vec4MulMtx(&vertices_[i].x, &posColors_[i].pos.x, trans);
        }
    }

    renderState_ = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | blendFunc_.toValue();
    if (flags_.isOn(DrawNode::DepthWrite))
    {
        renderState_ |= BGFX_STATE_DEPTH_TEST_LESS;
    }

    SharedRendererManager.setCurrent(SharedDrawRenderer.getTarget());
    SharedDrawRenderer.push(vertices_, indices_, renderState_);
}

void DrawNode::drawPoint(const Vec2& pos, const float pointSize, const Color4F &color)
{
    /*ensureCapacityGLPoint(1);

    V2F_C4B_T2F *point = (V2F_C4B_T2F*)(_bufferGLPoint + _bufferCountGLPoint);
    V2F_C4B_T2F a = { position, Color4B(color), Tex2F(pointSize,0) };
    *point = a;

    _bufferCountGLPoint += 1;
    _dirtyGLPoint = true;*/
}

void DrawNode::drawPoints(const Vec2 *position, unsigned int numberOfPoints, const Color4F &color)
{
    drawPoints(position, numberOfPoints, 1.0, color);
}

void DrawNode::drawPoints(const Vec2 *position, unsigned int numberOfPoints, const float pointSize, const Color4F &color)
{
    /*ensureCapacityGLPoint(numberOfPoints);

    V2F_C4B_T2F *point = (V2F_C4B_T2F*)(_bufferGLPoint + _bufferCountGLPoint);

    for(unsigned int i=0; i < numberOfPoints; i++,point++)
    {
        V2F_C4B_T2F a = {position[i], Color4B(color), Tex2F(pointSize,0)};
        *point = a;
    }

    _bufferCountGLPoint += numberOfPoints;
    _dirtyGLPoint = true;*/
}

void DrawNode::drawLine(const Vec2 &origin, const Vec2 &destination, const Color4F &color)
{
    /*ensureCapacityGLLine(2);

    V2F_C4B_T2F *point = (V2F_C4B_T2F*)(_bufferGLLine + _bufferCountGLLine);

    V2F_C4B_T2F a = {origin, Color4B(color), Tex2F(0.0, 0.0)};
    V2F_C4B_T2F b = {destination, Color4B(color), Tex2F(0.0, 0.0)};

    *point = a;
    *(point+1) = b;

    _bufferCountGLLine += 2;
    _dirtyGLLine = true;*/
}

void DrawNode::drawRect(const Vec2 &origin, const Vec2 &destination, const Color4F &color)
{
    drawLine(Vec2(origin.x, origin.y), Vec2(destination.x, origin.y), color);
    drawLine(Vec2(destination.x, origin.y), Vec2(destination.x, destination.y), color);
    drawLine(Vec2(destination.x, destination.y), Vec2(origin.x, destination.y), color);
    drawLine(Vec2(origin.x, destination.y), Vec2(origin.x, origin.y), color);
}

void DrawNode::drawPoly(const Vec2 *poli, unsigned int numberOfPoints, bool closePolygon, const Color4F &color)
{
    /*uint32_t vertextCount;
    if(closePolygon)
    {
        vertextCount = 2 * numberOfPoints;
    }
    else
    {
        vertextCount = 2 * (numberOfPoints - 1);
    }
    posColors_.reserve(posColors_.size() + vertextCount);
    vertices_.reserve(vertices_.size() + vertextCount);

    Vec4 color4 = color.toVec4();
    V2F_C4B_T2F *point = (V2F_C4B_T2F*)(_bufferGLLine + _bufferCountGLLine);

    uint32_t i = 0;
    for(; i<numberOfPoints-1; i++)
    {
        V2F_C4B_T2F a = {poli[i], Color4B(color), Tex2F(0.0, 0.0)};
        V2F_C4B_T2F b = {poli[i+1], Color4B(color), Tex2F(0.0, 0.0)};

        *point = a;
        *(point+1) = b;
        point += 2;
    }
    if(closePolygon)
    {
        V2F_C4B_T2F a = {poli[i], Color4B(color), Tex2F(0.0, 0.0)};
        V2F_C4B_T2F b = {poli[0], Color4B(color), Tex2F(0.0, 0.0)};
        *point = a;
        *(point+1) = b;
    }

    _bufferCountGLLine += vertext_count;*/
}

void DrawNode::drawCircle(const Vec2& center, float radius, float angle, unsigned int segments, bool drawLineToCenter, float scaleX, float scaleY, const Color4F &color)
{
    const float coef = 2.0f * (float)M_PI/segments;

    Vec2 *vertices = new (std::nothrow) Vec2[segments+2];
    if( ! vertices )
        return;

    for(unsigned int i = 0;i <= segments; i++) {
        float rads = i*coef;
        float j = radius * cosf(rads + angle) * scaleX + center.x;
        float k = radius * sinf(rads + angle) * scaleY + center.y;

        vertices[i].x = j;
        vertices[i].y = k;
    }
    if(drawLineToCenter)
    {
        vertices[segments+1].x = center.x;
        vertices[segments+1].y = center.y;
        drawPoly(vertices, segments+2, true, color);
    }
    else
        drawPoly(vertices, segments+1, true, color);

    CC_SAFE_DELETE_ARRAY(vertices);
}

void DrawNode::drawCircle(const Vec2 &center, float radius, float angle, unsigned int segments, bool drawLineToCenter, const Color4F &color)
{
    drawCircle(center, radius, angle, segments, drawLineToCenter, 1.0f, 1.0f, color);
}

void DrawNode::drawQuadBezier(const Vec2 &origin, const Vec2 &control, const Vec2 &destination, unsigned int segments, const Color4F &color)
{
    Vec2* vertices = new (std::nothrow) Vec2[segments + 1];
    if( ! vertices )
        return;

    float t = 0.0f;
    for(unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1 - t, 2) * origin.x + 2.0f * (1 - t) * t * control.x + t * t * destination.x;
        vertices[i].y = powf(1 - t, 2) * origin.y + 2.0f * (1 - t) * t * control.y + t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    drawPoly(vertices, segments+1, false, color);

    CC_SAFE_DELETE_ARRAY(vertices);
}

void DrawNode::drawCubicBezier(const Vec2 &origin, const Vec2 &control1, const Vec2 &control2, const Vec2 &destination, unsigned int segments, const Color4F &color)
{
    Vec2* vertices = new (std::nothrow) Vec2[segments + 1];
    if( ! vertices )
        return;

    float t = 0;
    for (unsigned int i = 0; i < segments; i++)
    {
        vertices[i].x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * control1.x + 3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
        vertices[i].y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * control1.y + 3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
        t += 1.0f / segments;
    }
    vertices[segments].x = destination.x;
    vertices[segments].y = destination.y;

    drawPoly(vertices, segments+1, false, color);

    CC_SAFE_DELETE_ARRAY(vertices);
}

void DrawNode::drawCardinalSpline(PointArray *config, float tension,  unsigned int segments, const Color4F &color)
{
    Vec2* vertices = new (std::nothrow) Vec2[segments + 1];
    if( ! vertices )
        return;

    ssize_t p;
    float lt;
    float deltaT = 1.0f / config->count();

    for( unsigned int i=0; i < segments+1;i++) {

        float dt = (float)i / segments;

        // border
        if( dt == 1 ) {
            p = config->count() - 1;
            lt = 1;
        } else {
            p = dt / deltaT;
            lt = (dt - deltaT * (float)p) / deltaT;
        }

        // Interpolate
        Vec2 pp0 = config->getControlPointAtIndex(p-1);
        Vec2 pp1 = config->getControlPointAtIndex(p+0);
        Vec2 pp2 = config->getControlPointAtIndex(p+1);
        Vec2 pp3 = config->getControlPointAtIndex(p+2);

        Vec2 newPos = ccCardinalSplineAt( pp0, pp1, pp2, pp3, tension, lt);
        vertices[i].x = newPos.x;
        vertices[i].y = newPos.y;
    }

    drawPoly(vertices, segments+1, false, color);

    CC_SAFE_DELETE_ARRAY(vertices);
}

void DrawNode::drawCatmullRom(PointArray *points, unsigned int segments, const Color4F &color)
{
    drawCardinalSpline( points, 0.5f, segments, color);
}

void DrawNode::drawDot(const Vec2 &pos, float radius, const Color4F &color)
{
    const size_t vertexCount = 4;
    const size_t indexCount = 6;

    posColors_.reserve(posColors_.size() + vertexCount);
    vertices_.reserve(vertices_.size() + vertexCount);
    indices_.reserve(indices_.size() + indexCount);

    Vec4 color4 = color.toVec4();
    uint16_t start = static_cast<uint16_t>(vertices_.size());

    pushVertex({ pos.x - radius, pos.y - radius }, color4, { -1, 1 });
    pushVertex({ pos.x - radius, pos.y + radius }, color4, { -1, 1 });
    pushVertex({ pos.x + radius, pos.y + radius }, color4, { 1, 1 });
    pushVertex({ pos.x + radius, pos.y - radius }, color4, { 1, -1 });

    const uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };
    for (uint16_t index : indices)
    {
        indices_.push_back(start + index);
    }

    flags_.setOn(DrawNode::VertexColorDirty);
    flags_.setOn(DrawNode::VertexPosDirty);
}

void DrawNode::drawRect(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2& p4, const Color4F &color)
{
    drawLine(Vec2(p1.x, p1.y), Vec2(p2.x, p2.y), color);
    drawLine(Vec2(p2.x, p2.y), Vec2(p3.x, p3.y), color);
    drawLine(Vec2(p3.x, p3.y), Vec2(p4.x, p4.y), color);
    drawLine(Vec2(p4.x, p4.y), Vec2(p1.x, p1.y), color);
}

void DrawNode::drawSegment(const Vec2 &from, const Vec2 &to, float radius, const Color4F &color)
{
    const size_t vertexCount = 6 * 3;
    const size_t indexCount = vertexCount;
    posColors_.reserve(posColors_.size() + vertexCount);
    vertices_.reserve(vertices_.size() + vertexCount);
    indices_.reserve(indices_.size() + indexCount);

    Vec2 a = from;
    Vec2 b = to;

    Vec2 n = (b - a).getPerp().getNormalized();
    Vec2 t = n.getPerp();

    Vec2 nw = n * radius;
    Vec2 tw = t * radius;
    Vec2 v0 = b - (nw + tw);
    Vec2 v1 = b + (nw - tw);
    Vec2 v2 = b - nw;
    Vec2 v3 = b + nw;
    Vec2 v4 = a - nw;
    Vec2 v5 = a + nw;
    Vec2 v6 = a - (nw - tw);
    Vec2 v7 = a + nw + tw;

    uint16_t start = static_cast<uint16_t>(vertices_.size());
    Vec4 color4 = color.toVec4();

    pushVertex(v0, color4, -(n + t));
    pushVertex(v1, color4, n - t);
    pushVertex(v2, color4, -n);

    pushVertex(v3, color4, n);
    pushVertex(v1, color4, n - t);
    pushVertex(v2, color4, -n);

    pushVertex(v3, color4, n);
    pushVertex(v4, color4, -n);
    pushVertex(v2, color4, -n);

    pushVertex(v3, color4, n);
    pushVertex(v4, color4, -n);
    pushVertex(v5, color4, n);

    pushVertex(v6, color4, t - n);
    pushVertex(v4, color4, -n);
    pushVertex(v5, color4, n);

    pushVertex(v6, color4, t - n);
    pushVertex(v7, color4, n + t);
    pushVertex(v5, color4, n);

    for (uint16_t i = 0; i < indexCount; i++)
    {
        indices_.push_back(start + i);
    }

    flags_.setOn(DrawNode::VertexColorDirty);
    flags_.setOn(DrawNode::VertexPosDirty);
}

void DrawNode::drawPolygon(const Vec2 *verts, uint32_t count, const Color4F &fillColor, float borderWidth, const Color4F &borderColor)
{
    /*bool outline = (borderColor.a > 0.0f && borderWidth > 0.0f);
    const size_t triangleCount = outline ? 3 * count - 2 : count - 2;
    const size_t vertexCount = 3 * triangleCount;
    posColors_.reserve(vertexCount);
    vertices_.reserve(vertexCount);

    Vec4 fillColor4 = fillColor.toVec4();
    Vec4 borderColor4 = borderColor.toVec4();
    uint16_t start = static_cast<uint16_t>(vertices_.size());

    for (uint32_t i = 0; i < count - 2; ++i)
    {
        pushVertex(verts[0], fillColor4, Vec2::ZERO);
        pushVertex(verts[i+1], fillColor4, Vec2::ZERO);
        pushVertex(verts[i+2], fillColor4, Vec2::ZERO);
    }

    const size_t indexCount = vertices_.size() - start;
    indices_.reserve(indexCount);

    for (uint16_t i = 0; i < indexCount; i++)
    {
        indices_.push_back(start + i);
    }*/
    struct ExtrudeVerts { Vec2 offset, n; };
    std::vector<ExtrudeVerts> extrude(count, { Vec2::ZERO, Vec2::ZERO });
    for (uint32_t i = 0; i < count; ++i)
    {
        Vec2 v0 = verts[(i - 1 + count) % count];
        Vec2 v1 = verts[i];
        Vec2 v2 = verts[(i + 1) % count];
        
        Vec2 n1 = ((v1 - v0).getPerp()).getNormalized();
        Vec2 n2 = ((v2 - v1).getPerp()).getNormalized();

        Vec2 offset = (n1 + n2) * (1.0f / (n1.dot(n2) + 1.0f));
        extrude[i] = { offset, n2 };
    }
    bool outline = (borderColor.a > 0.0f && borderWidth > 0.0f);
    bool fillPoly = true;// (fillColor.a > 0);
    
    const size_t triangleCount = fillPoly ? count - 2 : 3 * count - 2;
    const size_t vertexCount = 3 * triangleCount;
    posColors_.reserve(vertexCount);
    vertices_.reserve(vertexCount);

    Vec4 fillColor4 = fillColor.toVec4();
    Vec4 borderColor4 = borderColor.toVec4();
    uint16_t start = static_cast<uint16_t>(vertices_.size());

    float inset = (outline ? 0.0f : 0.5f);

    for (uint32_t i = 0; i < count - 2; ++i)
    {
        Vec2 v0 = verts[0] - (extrude[0].offset * inset);
        Vec2 v1 = verts[i + 1] - (extrude[i + 1].offset * inset);
        Vec2 v2 = verts[i + 2] - (extrude[i + 2].offset * inset);

        pushVertex(v0, fillColor4, Vec2::ZERO);
        pushVertex(v1, fillColor4, Vec2::ZERO);
        pushVertex(v2, fillColor4, Vec2::ZERO);
    }
    if (!fillPoly)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            uint32_t j = (i + 1) % count;
            Vec2 v0 = verts[i];
            Vec2 v1 = verts[j];
            Vec2 n0 = extrude[i].n;
            Vec2 offset0 = extrude[i].offset;
            Vec2 offset1 = extrude[j].offset;
            if (outline)
            {
                Vec2 inner0 = v0 - (offset0 * borderWidth);
                Vec2 inner1 = v1 - (offset1 * borderWidth);
                Vec2 outer0 = v0 + (offset0 * borderWidth);
                Vec2 outer1 = v1 + (offset1 * borderWidth);

                pushVertex(inner0, borderColor4, -n0);
                pushVertex(inner1, borderColor4, -n0);
                pushVertex(outer1, borderColor4, n0);

                pushVertex(inner0, borderColor4, -n0);
                pushVertex(outer0, borderColor4, n0);
                pushVertex(outer1, borderColor4, n0);
            }
            else
            {
                Vec2 inner0 = v0 - (offset0 * 0.5f);
                Vec2 inner1 = v1 - (offset1 * 0.5f);
                Vec2 outer0 = v0 + (offset0 * 0.5f);
                Vec2 outer1 = v1 + (offset1 * 0.5f);

                pushVertex(inner0, fillColor4, Vec2::ZERO);
                pushVertex(inner1, fillColor4, Vec2::ZERO);
                pushVertex(outer1, fillColor4, n0);

                pushVertex(inner0, fillColor4, Vec2::ZERO);
                pushVertex(outer0, fillColor4, n0);
                pushVertex(outer1, fillColor4, n0);
            }
        }
    }

    const size_t indexCount = vertices_.size() - start;
    indices_.reserve(indexCount);

    for (uint16_t i = 0; i < indexCount; i++)
    {
        indices_.push_back(start + i);
    }

    flags_.setOn(DrawNode::VertexColorDirty);
    flags_.setOn(DrawNode::VertexPosDirty);
}

void DrawNode::drawSolidRect(const Vec2 &origin, const Vec2 &destination, const Color4F &color)
{
    Vec2 vertices[] = {
        origin,
        Vec2(destination.x, origin.y),
        destination,
        Vec2(origin.x, destination.y)
    };

    drawSolidPoly(vertices, 4, color );
}

void DrawNode::drawSolidPoly(const Vec2 *poli, unsigned int numberOfPoints, const Color4F &color)
{
    drawPolygon(poli, numberOfPoints, color, 0.0, Color4F(0.0, 0.0, 0.0, 0.0));
}

void DrawNode::drawSolidCircle(const Vec2& center, float radius, float angle, unsigned int segments, float scaleX, float scaleY, const Color4F &color)
{
    const float coef = 2.0f * (float)M_PI/segments;

    Vec2 *vertices = new (std::nothrow) Vec2[segments];
    if( ! vertices )
        return;

    for(unsigned int i = 0;i < segments; i++)
    {
        float rads = i*coef;
        GLfloat j = radius * cosf(rads + angle) * scaleX + center.x;
        GLfloat k = radius * sinf(rads + angle) * scaleY + center.y;

        vertices[i].x = j;
        vertices[i].y = k;
    }

    drawSolidPoly(vertices, segments, color);

    CC_SAFE_DELETE_ARRAY(vertices);
}

void DrawNode::drawSolidCircle( const Vec2& center, float radius, float angle, unsigned int segments, const Color4F& color)
{
    drawSolidCircle(center, radius, angle, segments, 1.0f, 1.0f, color);
}

void DrawNode::drawTriangle(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Color4F &color)
{
    uint32_t vertexCount = 3;
    posColors_.reserve(posColors_.size() + vertexCount);
    vertices_.reserve(vertices_.size() + vertexCount);

    Vec4 color4 = color.toVec4();
    pushVertex({ p1.x, p1.y }, color4, Vec2::ZERO);
    pushVertex({ p2.x, p2.y }, color4, Vec2::ZERO);
    pushVertex({ p3.x, p3.y }, color4, Vec2::ZERO);

    flags_.setOn(DrawNode::VertexColorDirty);
    flags_.setOn(DrawNode::VertexPosDirty);
}

void DrawNode::clear()
{
    posColors_.clear();
    vertices_.clear();
    indices_.clear();
}



void DrawNode::setLineWidth(int lineWidth)
{
    
}

float DrawNode::getLineWidth()
{
    return 0;
}

const Mat4& DrawNode::getNodeToParentTransform() const
{
    if (flags_.isOn(Node::WorldDirty))
    {
        flags_.setOn(DrawNode::VertexPosDirty);
    }
    return Node::getNodeToParentTransform();
}

NS_CC_END

