#pragma once

#include "base/ccTypes.h"

NS_CC_BEGIN

class Node;
class SpriteProgram;
class Program;
class Texture2D;

class CC_DLL IRenderer
{
public:
    virtual void render();
};

class CC_DLL Renderer : public IRenderer
{
    friend class RendererManager;
public:
    PROPERTY_READONLY(SpriteProgram*, DefaultProgram);
    PROPERTY_READONLY(SpriteProgram*, DefaultProgramMVP);
    PROPERTY_READONLY(SpriteProgram*, LightProgram);
    PROPERTY_READONLY(SpriteProgram*, GrayProgram);
    PROPERTY_READONLY(SpriteProgram*, AlphaTestProgram);
    PROPERTY_READONLY(SpriteProgram*, NormalProgram);
    PROPERTY_READONLY(SpriteProgram*, OutlineProgram);
    PROPERTY_READONLY(SpriteProgram*, GradientProgram);
    PROPERTY_READONLY(SpriteProgram*, GradientOutlineProgram);
    PROPERTY_READONLY(SpriteProgram*, DistanceField);
    PROPERTY_READONLY(SpriteProgram*, DistanceFieldGlowProgram);
    void render() override;
    void push(V3F_C4B_T2F* verts, uint32_t vsize, uint16_t* indices, uint32_t isize, SpriteProgram* program, Texture2D* texture, uint64_t state, uint32_t flags);
    void push(V3F_C4B_T2F* verts, uint32_t vsize, uint16_t* indices, uint32_t isize, SpriteProgram* program, Texture2D* texture, uint64_t state, uint32_t flags, const float* modelWorld);
    void push(V3F_C4B_T2F* verts, uint32_t vsize, uint16_t* indices, uint32_t isize, SpriteProgram* program, Texture2D* texture, uint64_t state, uint32_t flags, const Mat4& modelWorld);
    void push(V3F_C4B_T2F_Quad* quads, uint32_t quadsCount, SpriteProgram* program, Texture2D* texture, uint64_t state, uint32_t flags, const Mat4& modelWorld);
    /** returns whether or not a rectangle is visible or not */
    bool checkVisibility(const Mat4& transform, const Size& size) { return true; }
protected:
    Renderer();
private:
    SmartPtr<SpriteProgram> defaultProgram_;
    SmartPtr<SpriteProgram> defaultProgramMVP_;
    SmartPtr<SpriteProgram> lightProgram_;
    SmartPtr<SpriteProgram> grayProgram_;
    SmartPtr<SpriteProgram> alphaTestProgram;
    SmartPtr<SpriteProgram> normalProgram_;
    SmartPtr<SpriteProgram> outlineProgram_;
    SmartPtr<SpriteProgram> gradientProgram_;
    SmartPtr<SpriteProgram> gradientOutlineProgram_;
    SmartPtr<SpriteProgram> distanceFieldProgram_;
    SmartPtr<SpriteProgram> distanceFieldGlowProgram_;

    std::vector<V3F_C4B_T2F> vertices_;
    std::vector<uint16_t> indices_;
    uint64_t lastState_;
    uint32_t lastFlags_;
    Texture2D* lastTexture_;
    SpriteProgram* lastProgram_;
    SINGLETON_REF(Renderer, RendererManager);
};

#define SharedRenderer \
    cocos2d::Singleton<cocos2d::Renderer>::shared()

class DrawRenderer : public IRenderer
{
public:
    PROPERTY_READONLY(Program*, DefaultProgram);
    virtual ~DrawRenderer() {};
    virtual void render() override;
    void push(std::vector<DrawVertex>& verts, std::vector<uint16_t>& indices, uint64_t renderState);
protected:
    DrawRenderer();
private:
    SmartPtr<Program> defaultProgram_;
    uint64_t lastState_;
    std::vector<DrawVertex> vertices_;
    std::vector<uint16_t> indices_;
    SINGLETON_REF(DrawRenderer, RendererManager);
};

#define SharedDrawRenderer \
    cocos2d::Singleton<DrawRenderer>::shared()

class LineRenderer : public IRenderer
{
public:
    PROPERTY_READONLY(Program*, DefaultProgram);
    PROPERTY_READONLY(Program*, GraphicsProgram);
    virtual ~LineRenderer() { }
    virtual void render() override;
    void push(VecVertex* verts, uint32_t vsize, uint16_t* indices, uint32_t isize, uint64_t renderState, Program* program, const float* modelWorld);
    //void push(Line* line);
protected:
    LineRenderer();
private:
    SmartPtr<Program> defaultProgram_;
    SmartPtr<Program> graphicsProgram_;
    uint64_t lastState_;
    Program* lastProgram_;
    std::vector<VecVertex> vertices_;
    std::vector<uint16_t> indices_;
    SINGLETON_REF(LineRenderer, RendererManager);
};

#define SharedLineRenderer \
    cocos2d::Singleton<cocos2d::LineRenderer>::shared()

class CC_DLL RendererManager
{
public:
    PROPERTY(IRenderer*, Current);
    PROPERTY_READONLY(uint32_t, CurrentStencilState);
    PROPERTY_BOOL(Grouping);

    void flush();

    template<typename Func>
    void sandwichStencilState(uint32_t stencilState, const Func& call)
    {
        pushStencilState(stencilState);
        call();
        popStencilState();
    }
    void pushGroupItem(Node* item);

    template<typename Func>
    void pushGroup(uint32_t capacity, const Func& call)
    {
        pushGroup(capacity);
        call();
        popGroup();
    }

protected:
    RendererManager();
    void pushStencilState(uint32_t stencilState);
    void popStencilState();
    void pushGroup(uint32_t capacity);
    void popGroup();
private:
    std::stack<uint32_t> stencilStates_;
    IRenderer* currentRenderer_;
    std::stack<Own<std::vector<Node*>>> renderGroups_;

    static RendererManager* s_rendererManager;
    SINGLETON_REF(RendererManager, BGFXCocos);
};

#define SharedRendererManager \
	cocos2d::Singleton<cocos2d::RendererManager>::shared()

NS_CC_END