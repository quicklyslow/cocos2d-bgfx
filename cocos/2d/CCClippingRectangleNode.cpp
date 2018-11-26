
#include "ccHeader.h"
#include "2d/CCClippingRectangleNode.h"
#include "base/CCDirector.h"
#include "platform/CCGLView.h"
#include "base/View.h"

NS_CC_BEGIN

ClippingRectangleNode* ClippingRectangleNode::create(const Rect& clippingRegion)
{
    ClippingRectangleNode* node = new (std::nothrow) ClippingRectangleNode();
    if (node && node->init()) {
        node->setClippingRegion(clippingRegion);
        node->autorelease();
    } else {
        CC_SAFE_DELETE(node);
    }

    return node;
}

ClippingRectangleNode* ClippingRectangleNode::create()
{
    ClippingRectangleNode* node = new (std::nothrow) ClippingRectangleNode();
    if (node && node->init()) {
        node->autorelease();
    } else {
        CC_SAFE_DELETE(node);
    }

    return node;
}

void ClippingRectangleNode::setClippingRegion(const Rect &clippingRegion)
{
    _clippingRegion = clippingRegion;
}

void ClippingRectangleNode::onBeforeVisitScissor()
{
    if (_clippingEnabled) {

        float scaleX = _scaleX;
        float scaleY = _scaleY;
        Node *parent = this->getParent();
        while (parent) {
            scaleX *= parent->getScaleX();
            scaleY *= parent->getScaleY();
            parent = parent->getParent();
        }

        const Point pos = convertToWorldSpace(Point(_clippingRegion.origin.x, _clippingRegion.origin.y));
        GLView* glView = SharedDirector.getOpenGLView();
        const Rect& viewportRect = glView->getViewPortRect();
        bgfx::setViewScissor(SharedView.getId(), 
            (uint16_t)(pos.x * _scaleX + viewportRect.origin.x),
            (uint16_t)(pos.y * _scaleY + viewportRect.origin.y),
            (uint16_t)(_clippingRegion.size.width * scaleX * glView->getScaleX()),
            (uint16_t)(_clippingRegion.size.height * scaleY * glView->getScaleY()));
    }
}

void ClippingRectangleNode::onAfterVisitScissor()
{
    if (_clippingEnabled)
    {
        bgfx::setViewScissor(SharedView.getId());
    }
}

void ClippingRectangleNode::visit(IRenderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    onBeforeVisitScissor();
    Node::visit(renderer, parentTransform, parentFlags);
    onAfterVisitScissor();
}

NS_CC_END

