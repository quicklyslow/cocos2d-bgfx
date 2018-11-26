#include "ccHeader.h"
#include "Renderer.h"
#include "2d/CCNode.h"
#include "base/View.h"
#include "base/CCDirector.h"
#include "base/Camera.h"
#include "renderer/Program.h"
#include "renderer/CCTexture2D.h"

NS_CC_BEGIN


void IRenderer::render()
{
    uint32_t stencilState = SharedRendererManager.getCurrentStencilState();
    if (stencilState != BGFX_STENCIL_NONE)
    {
        bgfx::setStencil(stencilState, stencilState);
    }
}

Renderer::Renderer()
    : defaultProgram_(SpriteProgram::create("vs_spritemodel.bin"_slice, "fs_sprite.bin"_slice))
    , defaultProgramMVP_(SpriteProgram::create("vs_label.bin"_slice, "fs_sprite.bin"_slice))
    , lightProgram_(SpriteProgram::create("vs_spritemodel.bin"_slice, "fs_spritelight.bin"_slice))
    , grayProgram_(SpriteProgram::create("vs_spritemodel.bin"_slice, "fs_spritegray.bin"_slice))
    , alphaTestProgram(SpriteProgram::create("vs_spritemodel.bin"_slice, "fs_spritealphatest.bin"_slice))
    , normalProgram_(SpriteProgram::create("vs_label.bin"_slice, "fs_labelnormal.bin"_slice))
    , outlineProgram_(SpriteProgram::create("vs_label.bin"_slice, "fs_labeloutline.bin"_slice))
    , gradientProgram_(SpriteProgram::create("vs_labelposition.bin"_slice, "fs_labelgradient.bin"_slice))
    , gradientOutlineProgram_(SpriteProgram::create("vs_labelposition.bin"_slice, "fs_labelgradientoutline.bin"_slice))
    //, distanceFieldProgram_(SpriteProgram::create("vs_labelposition.bin"_slice, "fs_labeldf.bin"_slice))
    //, distanceFieldGlowProgram_(SpriteProgram::create("vs_labelposition.bin"_slice, "fs_labeldfglow.bin"_slice))
    , lastProgram_(nullptr)
    , lastTexture_(nullptr)
    , lastState_(0)
    , lastFlags_(UINT32_MAX)
{

}

SpriteProgram* Renderer::getDefaultProgram() const
{
    return defaultProgram_;
}

SpriteProgram* Renderer::getDefaultProgramMVP() const
{
    return defaultProgramMVP_;
}

SpriteProgram* Renderer::getLightProgram() const
{
    return lightProgram_;
}

SpriteProgram* Renderer::getGrayProgram() const
{
    return grayProgram_;
}

SpriteProgram* Renderer::getAlphaTestProgram() const
{
    return alphaTestProgram;
}

SpriteProgram* Renderer::getNormalProgram() const
{
    return normalProgram_;
}

SpriteProgram* Renderer::getOutlineProgram() const
{
    return outlineProgram_;
}

SpriteProgram* Renderer::getGradientProgram() const
{
    return gradientProgram_;
}

SpriteProgram* Renderer::getGradientOutlineProgram() const
{
    return gradientOutlineProgram_;
}

SpriteProgram* Renderer::getDistanceField() const
{
    return distanceFieldProgram_;
}

SpriteProgram* Renderer::getDistanceFieldGlowProgram() const
{
    return distanceFieldGlowProgram_;
}

void Renderer::push(V3F_C4B_T2F* verts, uint32_t vsize,
    uint16_t* indices, uint32_t isize,
    SpriteProgram* program, Texture2D* texture, 
    uint64_t state, uint32_t flags)
{
    if (program != lastProgram_ || texture != lastTexture_ || state != lastState_ || flags != lastFlags_)
    {
        render();
    }

    lastProgram_ = program;
    lastTexture_ = texture;
    lastState_ = state;
    lastFlags_ = flags;

    size_t oldVertSize = vertices_.size();
    vertices_.resize(oldVertSize + vsize);
    std::memcpy(vertices_.data() + oldVertSize, verts, sizeof(verts[0]) * vsize);

    size_t oldIndexSize = indices_.size();
    indices_.resize(oldIndexSize + isize);

    for (size_t i = 0; i < isize; ++i)
    {
        indices_[oldIndexSize + i] = indices[i] + oldVertSize;
    }
}

void Renderer::push(V3F_C4B_T2F* verts, uint32_t vsize,
    uint16_t* indices, uint32_t isize,
    SpriteProgram* program, Texture2D* texture,
    uint64_t state, uint32_t flags, const float* modelWorld)
{
    if (modelWorld || program != lastProgram_ || texture != lastTexture_ || state != lastState_ || flags != lastFlags_)
    {
        render();
    }
    lastProgram_ = program;
    lastTexture_ = texture;
    lastState_ = state;
    lastFlags_ = flags;

    size_t oldVertSize = vertices_.size();
    vertices_.resize(oldVertSize + vsize);
    std::memcpy(vertices_.data() + oldVertSize, verts, sizeof(verts[0]) * vsize);

    size_t oldIndexSize = indices_.size();
    indices_.resize(oldIndexSize + isize);
    for (size_t i = 0; i < isize; ++i)
    {
        indices_[oldIndexSize + i] = indices[i] + oldVertSize;
    }

    if (modelWorld)
    {
        bgfx::setTransform(modelWorld);
        render();
    }
}

void Renderer::push(V3F_C4B_T2F* verts, uint32_t vsize, 
    uint16_t* indices, uint32_t isize,
    SpriteProgram* program, Texture2D* texture, 
    uint64_t state, uint32_t flags, const Mat4& modelWorld)
{
    if (program != lastProgram_ || texture != lastTexture_ || state != lastState_ || flags != lastFlags_)
    {
        render();
    }

    lastProgram_ = program;
    lastTexture_ = texture;
    lastState_ = state;
    lastFlags_ = flags;

    size_t oldVertSize = vertices_.size();
    vertices_.resize(oldVertSize + vsize);
    std::memcpy(vertices_.data() + oldVertSize, verts, sizeof(verts[0]) * vsize);
    for (size_t i = 0; i < vsize; ++i)
    {
        modelWorld.transformPoint(&vertices_[oldVertSize + i].vertices);
    }

    size_t oldIndexSize = indices_.size();
    indices_.resize(oldIndexSize + isize);

    for (size_t i = 0; i < isize; ++i)
    {
        indices_[oldIndexSize + i] = indices[i] + oldVertSize;
    }
}

void Renderer::push(V3F_C4B_T2F_Quad* quads, uint32_t quadsCount,
    SpriteProgram* program, Texture2D* texture, 
    uint64_t state, uint32_t flags, const Mat4& modelWorld)
{
    if (program != lastProgram_ || texture != lastTexture_ || state != lastState_ || flags != lastFlags_)
    {
        render();
    }

    lastProgram_ = program;
    lastTexture_ = texture;
    lastState_ = state;
    lastFlags_ = flags;

    uint32_t vsize = quadsCount * 4;
    uint32_t isize = quadsCount * 6;

    size_t oldVertSize = vertices_.size();
    vertices_.resize(oldVertSize + vsize);
    std::memcpy(vertices_.data() + oldVertSize, &quads->tl, sizeof(V3F_C4B_T2F) * vsize);
    for (size_t i = 0; i < vsize / 4; ++i)
    {
        modelWorld.transformPoint(quads[i].tl.vertices, &vertices_[oldVertSize + i * 4 + 0].vertices);
        modelWorld.transformPoint(quads[i].bl.vertices, &vertices_[oldVertSize + i * 4 + 1].vertices);
        modelWorld.transformPoint(quads[i].tr.vertices, &vertices_[oldVertSize + i * 4 + 2].vertices);
        modelWorld.transformPoint(quads[i].br.vertices, &vertices_[oldVertSize + i * 4 + 3].vertices);
    }

    size_t oldIndexSize = indices_.size();
    indices_.resize(oldIndexSize + isize);
    for (size_t i = 0; i < isize / 6; ++i)
    {
        indices_[oldIndexSize + i * 6 + 0] = i * 4 + 0 + oldVertSize;
        indices_[oldIndexSize + i * 6 + 1] = i * 4 + 1 + oldVertSize;
        indices_[oldIndexSize + i * 6 + 2] = i * 4 + 2 + oldVertSize;
        indices_[oldIndexSize + i * 6 + 3] = i * 4 + 3 + oldVertSize;
        indices_[oldIndexSize + i * 6 + 4] = i * 4 + 2 + oldVertSize;
        indices_[oldIndexSize + i * 6 + 5] = i * 4 + 1 + oldVertSize;
    }
}

void Renderer::render()
{
    if (!vertices_.empty())
    {
        bgfx::TransientVertexBuffer vertexBuffer;
        bgfx::TransientIndexBuffer indexBuffer;
        uint32_t vertexCount = static_cast<uint32_t>(vertices_.size());
        uint32_t indexCount = static_cast<uint32_t>(indices_.size());
        if (bgfx::allocTransientBuffers(
            &vertexBuffer, V3F_C4B_T2F::ms_decl, vertexCount,
            &indexBuffer, indexCount))
        {
            IRenderer::render();
            std::memcpy(vertexBuffer.data, vertices_.data(), vertexCount * sizeof(vertices_[0]));
            bgfx::setVertexBuffer(0, &vertexBuffer);
            std::memcpy(indexBuffer.data, indices_.data(), indexCount * sizeof(indices_[0]));
            bgfx::setIndexBuffer(&indexBuffer);

            uint8_t viewId = SharedView.getId();
            //Mat4 viewProj; //set at director draw
            //bx::mtxMul(viewProj, SharedDirector.getCamera()->getView(), SharedView.getProjection());
            //bgfx::setViewTransform(viewId, SharedDirector.getCamera()->getView(), SharedView.getProjection());
            bgfx::setState(lastState_);
            bgfx::setTexture(0, lastProgram_->getSampler(), lastTexture_->getHandle(), lastFlags_);
            bgfx::submit(viewId, lastProgram_->apply());
        }
        else
        {
            CCLOG("not enough transient buffer for %d vertices, %d indices.", vertexCount, indexCount);
        }
        vertices_.clear();
        indices_.clear();
        lastProgram_ = nullptr;
        lastTexture_ = nullptr;
        lastState_ = 0;
        lastFlags_ = UINT32_MAX;
    }
}

DrawRenderer::DrawRenderer()
    :defaultProgram_(Program::create("vs_draw.bin"_slice, "fs_draw.bin"_slice))
{

}

Program* DrawRenderer::getDefaultProgram() const
{
    return defaultProgram_;
}

void DrawRenderer::push(std::vector<DrawVertex>& verts, std::vector<uint16_t>& indices, uint64_t renderState)
{
    if (renderState != lastState_)
    {
        render();
    }
    lastState_ = renderState;

    uint16_t start = static_cast<uint16_t>(vertices_.size());
    vertices_.reserve(vertices_.size() + verts.size());
    vertices_.insert(vertices_.end(), verts.begin(), verts.end());

    indices_.reserve(indices_.size() + indices.size());
    for (const auto& index : indices)
    {
        indices_.push_back(start + index);
    }
}

void DrawRenderer::render()
{
    if (!vertices_.empty())
    {
        bgfx::TransientVertexBuffer vertexBuffer;
        bgfx::TransientIndexBuffer indexBuffer;
        uint32_t vertexCount = static_cast<uint32_t>(vertices_.size());
        uint32_t indexCount = static_cast<uint32_t>(indices_.size());
        if (bgfx::allocTransientBuffers(
            &vertexBuffer, DrawVertex::ms_decl, vertexCount,
            &indexBuffer, indexCount))
        {
            IRenderer::render();
            std::memcpy(vertexBuffer.data, vertices_.data(), vertices_.size() * sizeof(vertices_[0]));
            std::memcpy(indexBuffer.data, indices_.data(), indices_.size() * sizeof(indices_[0]));
            bgfx::setVertexBuffer(0, &vertexBuffer);
            bgfx::setIndexBuffer(&indexBuffer);
            bgfx::setState(lastState_);
            uint8_t viewId = SharedView.getId();
            bgfx::submit(viewId, defaultProgram_->apply());
        }
        else
        {
            CCLOG("not enough transient buffer for %D vertices, %d indices.", vertexCount, indexCount);
        }
        vertices_.clear();
        indices_.clear();
        lastState_ = BGFX_STATE_NONE;
    }
}

LineRenderer::LineRenderer()
    :defaultProgram_(Program::create("vs_poscolor.bin"_slice, "fs_poscolor.bin"_slice))
    , graphicsProgram_(Program::create("vs_graphics.bin"_slice, "fs_graphics.bin"_slice))
    , lastState_(BGFX_STATE_NONE)
    , lastProgram_(nullptr)
{

}

Program* LineRenderer::getDefaultProgram() const
{
    return defaultProgram_;
}

Program* LineRenderer::getGraphicsProgram() const
{
    return graphicsProgram_;
}

void LineRenderer::push(VecVertex* verts, uint32_t vsize, uint16_t* indices, uint32_t isize, uint64_t renderState, Program* program, const float* modelWorld)
{
    if (modelWorld || program != lastProgram_ || renderState != lastState_)
    {
        render();
    }
    lastState_ = renderState;
    lastProgram_ = program;

    uint16_t oldVertSize = static_cast<uint16_t>(vertices_.size());
    vertices_.resize(oldVertSize + vsize);
    std::memcpy(vertices_.data() + oldVertSize, verts, sizeof(verts[0]) * vsize);

    uint16_t oldIndexSize = indices_.size();
    indices_.resize(oldIndexSize + isize);
    for (size_t i = 0; i < isize; ++i)
    {
        indices_[oldIndexSize + i] = indices[i] + oldVertSize;
    }

    if (modelWorld)
    {
        bgfx::setTransform(modelWorld);
        render();
    }
}

void LineRenderer::render()
{
    if (!vertices_.empty())
    {
        bgfx::TransientVertexBuffer vertexBuffer;
        bgfx::TransientIndexBuffer indexBuffer;
        uint32_t vertexCount = static_cast<uint32_t>(vertices_.size());
        uint32_t indexCount = static_cast<uint32_t>(indices_.size());
        if (bgfx::allocTransientBuffers(
            &vertexBuffer, VecVertex::ms_decl, vertexCount,
            &indexBuffer, indexCount))
        {
            IRenderer::render();
            std::memcpy(vertexBuffer.data, vertices_.data(), vertices_.size() * sizeof(vertices_[0]));
            std::memcpy(indexBuffer.data, indices_.data(), indices_.size() * sizeof(indices_[0]));
            bgfx::setVertexBuffer(0, &vertexBuffer);
            bgfx::setIndexBuffer(&indexBuffer);
            bgfx::setState(lastState_);
            uint8_t viewId = SharedView.getId();
            bgfx::submit(viewId, lastProgram_->apply());
        }
        else
        {
            CCLOG("not enough transient buffer for %D vertices, %d indices.", vertexCount, indexCount);
        }
        vertices_.clear();
        indices_.clear();
        lastState_ = BGFX_STATE_NONE;
    }
}

RendererManager::RendererManager()
    :currentRenderer_(nullptr)
{

}

void RendererManager::setCurrent(IRenderer* r)
{
    if (currentRenderer_ && currentRenderer_ != r)
    {
        currentRenderer_->render();
    }
    currentRenderer_ = r;
}

IRenderer* RendererManager::getCurrent() const
{
    return currentRenderer_;
}

uint32_t RendererManager::getCurrentStencilState() const
{
    return stencilStates_.empty() ? BGFX_STENCIL_NONE : stencilStates_.top();
}

void RendererManager::flush()
{
    if (currentRenderer_)
    {
        currentRenderer_->render();
        currentRenderer_ = nullptr;
    }
}

void RendererManager::pushGroupItem(Node* item)
{
    auto renderGroup = renderGroups_.top().get();
    renderGroup->push_back(item);
}

void RendererManager::pushStencilState(uint32_t stencilState)
{
    stencilStates_.push(stencilState);
}

void RendererManager::popStencilState()
{
    stencilStates_.pop();
}

void RendererManager::pushGroup(uint32_t capacity)
{
    renderGroups_.push(New<std::vector<Node*>>());
    renderGroups_.top()->reserve(capacity);
}

void RendererManager::popGroup()
{
    auto renderGroup = renderGroups_.top().get();
    std::stable_sort(renderGroup->begin(), renderGroup->end(), [](Node* l, Node* r) -> bool {
        return l->getGlobalZOrder() < r->getGlobalZOrder();
    });
    for (Node* node : *renderGroup)
    {
        node->visit();
    }
    renderGroups_.pop();
}

NS_CC_END