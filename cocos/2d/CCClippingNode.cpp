/*
 * Copyright (c) 2012      Pierre-David Bélanger
 * Copyright (c) 2012      cocos2d-x.org
 * Copyright (c) 2013-2016 Chukong Technologies Inc.
 *
 * cocos2d-x: http://www.cocos2d-x.org
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include "ccHeader.h"
#include "2d/CCClippingNode.h"
#include "renderer/Renderer.h"
#include "renderer/Program.h"
#include "base/CCDirector.h"
#include "base/View.h"
#include "base/CCScriptSupport.h"
#include "editor-support/creator/CCScale9Sprite.h"


NS_CC_BEGIN

int ClippingNode::_layer = -1;


ClippingNode::ClippingNode()
:_alphaThreshold(1.0f)
,_stencil(nullptr)
{
}

ClippingNode::~ClippingNode()
{

}

ClippingNode* ClippingNode::create()
{
    ClippingNode *ret = new (std::nothrow) ClippingNode();
    if (ret && ret->init())
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }

    return ret;
}

ClippingNode* ClippingNode::create(Node *pStencil)
{
    ClippingNode *ret = new (std::nothrow) ClippingNode();
    if (ret && ret->init(pStencil))
    {
        ret->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(ret);
    }

    return ret;
}

bool ClippingNode::init()
{
    return init(nullptr);
}

bool ClippingNode::init(Node *stencil)
{
    setStencil(stencil);
    return true;
}

void ClippingNode::onEnter()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == ccScriptType::kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnter))
            return;
    }
#endif

    Node::onEnter();

    if (_stencil != nullptr)
    {
        _stencil->onEnter();
    }
    else
    {
        CCLOG("ClippingNode warning: _stencil is nil.");
    }
}

void ClippingNode::onEnterTransitionDidFinish()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == ccScriptType::kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnterTransitionDidFinish))
            return;
    }
#endif

    Node::onEnterTransitionDidFinish();

    if (_stencil != nullptr)
    {
        _stencil->onEnterTransitionDidFinish();
    }
}

void ClippingNode::onExitTransitionDidStart()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == ccScriptType::kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExitTransitionDidStart))
            return;
    }
#endif

    if (_stencil != nullptr)
    {
        _stencil->onExitTransitionDidStart();
    }

    Node::onExitTransitionDidStart();
}

void ClippingNode::onExit()
{
#if CC_ENABLE_SCRIPT_BINDING
    if (_scriptType == ccScriptType::kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExit))
            return;
    }
#endif

    if (_stencil != nullptr)
    {
        _stencil->onExit();
    }

    Node::onExit();
}

void ClippingNode::drawFullScreenStencil(uint8_t maskLayer, bool value)
{
    SharedRendererManager.flush();
    bgfx::TransientVertexBuffer vertexBuffer;
    bgfx::TransientIndexBuffer indexBuffer;
    if (bgfx::allocTransientBuffers(&vertexBuffer, V3F_C4B_T2F::ms_decl, 4,
        &indexBuffer, 6))
    {
        Size viewSize = SharedView.getSize();
        float width = viewSize.width;
        float height = viewSize.height;
        Vec4 pos[4] = {
            {0, height, 0, 1},
            {width, height, 0, 1},
            {0, 0, 0, 1},
            {width, 0, 0, 1}
        };
        V3F_C4B_T2F* vertices = reinterpret_cast<V3F_C4B_T2F*>(vertexBuffer.data);
        Mat4 ortho;
        bx::mtxOrtho(ortho, 0, width, 0, height, 0, 1000.0f, 0, bgfx::getCaps()->homogeneousDepth);
        for (int i = 0; i < 4; ++i)
        {
            bx::vec4MulMtx(&vertices[i].vertices.x, pos[i], ortho);
        }
        const uint16_t indices[] = { 0, 1, 2, 1, 3, 2 };
        std::memcpy(indexBuffer.data, indices, sizeof(indices[0]) * 6);
        uint32_t func = BGFX_STENCIL_TEST_NEVER |
            BGFX_STENCIL_FUNC_REF(maskLayer) | BGFX_STENCIL_FUNC_RMASK(maskLayer);
        uint32_t fail = value ? BGFX_STENCIL_OP_FAIL_S_REPLACE : BGFX_STENCIL_OP_FAIL_S_ZERO;
        uint32_t op = fail | BGFX_STENCIL_OP_FAIL_Z_KEEP | BGFX_STENCIL_OP_PASS_Z_KEEP;
        uint32_t stencil = func | op;
        bgfx::setStencil(stencil);
        bgfx::setVertexBuffer(0, &vertexBuffer);
        bgfx::setIndexBuffer(&indexBuffer);
        bgfx::setState(BGFX_STATE_NONE);
        uint8_t viewId = SharedView.getId();
        bgfx::submit(viewId, SharedLineRenderer.getDefaultProgram()->apply());
    }
}

void ClippingNode::drawStencil(uint8_t maskLayer, bool value)
{
    uint32_t func = BGFX_STENCIL_TEST_NEVER |
        BGFX_STENCIL_FUNC_REF(maskLayer) | BGFX_STENCIL_FUNC_RMASK(maskLayer);
    uint32_t fail = value ? BGFX_STENCIL_OP_FAIL_S_REPLACE : BGFX_STENCIL_OP_FAIL_S_ZERO;
    uint32_t op = fail | BGFX_STENCIL_OP_FAIL_Z_KEEP | BGFX_STENCIL_OP_PASS_Z_KEEP;
    SharedRendererManager.sandwichStencilState(func | op, [&]()
    {
        _stencil->visit(nullptr, _modelViewTransform, 1);
        SharedRendererManager.flush();
    });
}

void ClippingNode::setupAlphaTest()
{
    if (_stencil)
    {
        bool setup = _alphaThreshold < 1.0f;
        SpriteProgram* program = setup ? SharedRenderer.getAlphaTestProgram() : SharedRenderer.getDefaultProgram();
        creator::Scale9SpriteV2* scaleSp = CocosCast<creator::Scale9SpriteV2>(_stencil.get());
        if(scaleSp)
        {
            scaleSp->setProgram(program);
            scaleSp->setAlphaRef(_alphaThreshold);
        }
    }
}

void ClippingNode::visit(IRenderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    if (flags_.isOff(Node::Visible) || !hasContent())
        return;

    uint32_t flags = processParentFlags(parentTransform, parentFlags);

    if (!_stencil || !_stencil->isVisible())
    {
        if (flags_.isOn(ClippingNode::Inverted))
        {
            Node::visit(renderer, parentTransform, parentFlags);
        }
        return;
    }
    if (_layer + 1 == 8)
    {
        static bool once = true;
        if (once)
        {
            once = false;
            CCLOG("Nesting more than %d stencils is not supported. Everything will be drawn without stencil for this node and its childs.", 8);
        }
        Node::visit(renderer, parentTransform, parentFlags);
        return;
    }
    _layer++;
    uint32_t maskLayer = 1 << _layer;
    uint32_t maskLayerLess = maskLayer - 1;
    uint32_t maskLayerLessEqual = maskLayer | maskLayerLess;
    SharedRendererManager.flush();
    if (isInverted())
    {
        drawFullScreenStencil(maskLayer, true);
    }
    drawStencil(maskLayer, !isInverted());
    uint32_t func = BGFX_STENCIL_TEST_EQUAL | BGFX_STENCIL_FUNC_REF(maskLayerLessEqual) | BGFX_STENCIL_FUNC_RMASK(maskLayerLessEqual);
    uint32_t op = BGFX_STENCIL_OP_FAIL_S_KEEP | BGFX_STENCIL_OP_FAIL_Z_KEEP | BGFX_STENCIL_OP_PASS_Z_KEEP;
    SharedRendererManager.sandwichStencilState(func | op, [&]()
    {
        Node::visit(renderer, parentTransform, parentFlags);
        SharedRendererManager.flush();
    });
    if (isInverted())
    {
        drawFullScreenStencil(maskLayer, false);
    }
    else
    {
        drawStencil(maskLayer, false);
    }
    _layer--;
}

Node* ClippingNode::getStencil() const
{
    return _stencil;
}

void ClippingNode::setStencil(Node *stencil)
{
    //early out if the stencil is already set
    if (_stencil == stencil)
        return;
    
#if CC_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        if (_stencil)
            sEngine->releaseScriptObject(this, _stencil);
        if (stencil)
            sEngine->retainScriptObject(this, stencil);
    }
#endif // CC_ENABLE_GC_FOR_NATIVE_OBJECTS
    
    //cleanup current stencil
    if(_stencil != nullptr && _stencil->isRunning())
    {
        _stencil->onExitTransitionDidStart();
        _stencil->onExit();
    }
    
    //initialise new stencil
    _stencil = stencil;
    if(_stencil != nullptr && this->isRunning())
    {
        _stencil->setTransformTarget(this);
        _stencil->onEnter();
        if(this->_isTransitionFinished)
        {
            _stencil->onEnterTransitionDidFinish();
        }
    }
}

bool ClippingNode::hasContent() const
{
    return _children.size() > 0;
}

float ClippingNode::getAlphaThreshold() const
{
    return _alphaThreshold;
}

void ClippingNode::setAlphaThreshold(float var)
{
    _alphaThreshold = var;
    setupAlphaTest();
}

bool ClippingNode::isInverted() const
{
    return flags_.isOn(ClippingNode::Inverted);
}

void ClippingNode::setInverted(bool var)
{
    flags_.setFlag(ClippingNode::Inverted, var);
}


NS_CC_END

