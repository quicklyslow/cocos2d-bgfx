#pragma once

#include "base/Value.h"

NS_CC_BEGIN

class Shader;

class Program : public Ref
{
public:
    virtual ~Program();
    bool init();

    void set(String name, float var);
    void set(String name, float var1, float var2, float var3, float var4);
    void set(String name, const Vec4& var);
    void set(String name, const Mat4& var);
    TValue* get(String name) const;
    bgfx::ProgramHandle apply();
    CREATE_FUNC(Program);
protected:
    Program(Shader* vertShader, Shader* fragShader);
    Program(String vertShader, String fragShader);
private:
    class Uniform : public Ref
    {
    public:
        PROPERTY_READONLY(bgfx::UniformHandle, Handle);
        PROPERTY_READONLY(TValue*, Value);
        virtual ~Uniform();
        void apply();
        CREATE_FUNC(Uniform);
    protected:
        Uniform(bgfx::UniformHandle handle, TValue* value);
    private:
        bgfx::UniformHandle handle_;
        SmartPtr<TValue> value_;
    };
    SmartPtr<Shader> fragShader_;
    SmartPtr<Shader> vertShader_;
    bgfx::ProgramHandle program_;
    std::unordered_map<std::string, SmartPtr<Uniform>> uniforms_;
    COCOS_TYPE_OVERRIDE(Program);
};

class SpriteProgram : public Program
{
public:
    virtual ~SpriteProgram();
    bgfx::UniformHandle getSampler() const;
    CREATE_FUNC(SpriteProgram);
protected:
    SpriteProgram(Shader* vertShader, Shader* fragShader);
    SpriteProgram(String vertShader, String fragShader);
private:
    bgfx::UniformHandle sampler_;
    COCOS_TYPE_OVERRIDE(SpriteProgram);
};


NS_CC_END