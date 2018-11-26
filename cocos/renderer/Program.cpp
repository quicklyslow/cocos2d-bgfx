

#include "ccHeader.h"
#include "Program.h"
#include "CCShaderCache.h"

NS_CC_BEGIN

Program::Uniform::Uniform(bgfx::UniformHandle handle, TValue* value)
    :handle_(handle)
    , value_(value)
{

}

Program::Uniform::~Uniform()
{
    if (bgfx::isValid(handle_))
    {
        bgfx::destroy(handle_);
    }
}

bgfx::UniformHandle Program::Uniform::getHandle() const
{
    return handle_;
}

TValue* Program::Uniform::getValue() const
{
    return value_;
}

void Program::Uniform::apply()
{
    if (value_->as<float>())
    {
        float value = value_->as<float>()->get();
        bgfx::setUniform(handle_, Vec4{ value });
    }
    else if (value_->as<Vec4>())
    {
        const Vec4& value = value_->as<Vec4>()->get();
        bgfx::setUniform(handle_, value);
    }
    else if (value_->as<Mat4>())
    {
        const Mat4& value = value_->as<Mat4>()->get();
        bgfx::setUniform(handle_, value);
    }
}

Program::Program(Shader* vertShader, Shader* fragShader)
    :vertShader_(vertShader)
    , fragShader_(fragShader)
{

}

Program::Program(String vertShader, String fragShader)
    :vertShader_(SharedShaderCache.load(vertShader))
    , fragShader_(SharedShaderCache.load(fragShader))
{

}

Program::~Program()
{
    if (bgfx::isValid(program_))
    {
        bgfx::destroy(program_);
    }
}

bool Program::init()
{
    program_ = bgfx::createProgram(vertShader_->getHandle(), fragShader_->getHandle());
    return bgfx::isValid(program_);
}

void Program::set(String name, float var)
{
    std::string uname(name);
    auto it = uniforms_.find(uname);
    if (it != uniforms_.end())
    {
        it->second->getValue()->as<float>()->set(var);
    }
    else
    {
        bgfx::UniformHandle handle = bgfx::createUniform(uname.c_str(), bgfx::UniformType::Vec4);
        uniforms_.emplace(uname, Uniform::create(handle, TValue::create(var)));
    }
}

void Program::set(String name, float var1, float var2, float var3, float var4)
{
    set(name, Vec4{ var1, var2, var3, var4 });
}

void Program::set(String name, const Vec4& var)
{
    std::string uname(name);
    auto it = uniforms_.find(uname);
    if (it != uniforms_.end())
    {
        it->second->getValue()->as<Vec4>()->set(var);
    }
    else
    {
        bgfx::UniformHandle handle = bgfx::createUniform(uname.c_str(), bgfx::UniformType::Vec4);
        uniforms_.emplace(uname, Uniform::create(handle, TValue::create(var)));
    }
}

void Program::set(String name, const Mat4& var)
{
    std::string uname(name);
    auto it = uniforms_.find(uname);
    if (it != uniforms_.end())
    {
        it->second->getValue()->as<Mat4>()->set(var);
    }
    else
    {
        bgfx::UniformHandle handle = bgfx::createUniform(uname.c_str(), bgfx::UniformType::Mat4);
        uniforms_.emplace(uname, Uniform::create(handle, TValue::create(var)));
    }
}

TValue* Program::get(String name) const
{
    auto it = uniforms_.find(name);
    if (it != uniforms_.end())
    {
        return it->second->getValue();
    }
    return nullptr;
}

bgfx::ProgramHandle Program::apply()
{
    for (const auto& pair : uniforms_)
    {
        pair.second->apply();
    }
    return program_;
}

/*SpriteProgram*/

SpriteProgram::SpriteProgram(Shader* vertShader, Shader* fragShader)
    : Program(vertShader, fragShader)
    , sampler_(bgfx::createUniform("s_texColor", bgfx::UniformType::Int1))
{

}

SpriteProgram::SpriteProgram(String vertShader, String fragShader)
    : Program(vertShader, fragShader)
    , sampler_(bgfx::createUniform("s_texColor", bgfx::UniformType::Int1))
{

}

SpriteProgram::~SpriteProgram()
{
    if (bgfx::isValid(sampler_))
    {
        bgfx::destroy(sampler_);
    }
}

bgfx::UniformHandle SpriteProgram::getSampler() const
{
    return sampler_;
}

NS_CC_END