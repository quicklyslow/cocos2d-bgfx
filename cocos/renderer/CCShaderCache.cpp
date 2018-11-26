
#include "ccHeader.h"
#include "CCShaderCache.h"
#include "CCFileUtils.h"

NS_CC_BEGIN

Shader::Shader(bgfx::ShaderHandle handle) :
    handle_(handle)
{

}

Shader::~Shader()
{
    if (bgfx::isValid(handle_))
    {
        bgfx::destroy(handle_);
    }
}

bgfx::ShaderHandle Shader::getHandle() const
{
    return handle_;
}

// ShaderCache

ShaderCache::ShaderCache()
{

}

void ShaderCache::update(String name, Shader* shader)
{
    std::string shaderFile = FileUtils::getInstance()->fullPathForFilename(getShaderPath() + name);
    shaders_[shaderFile] = shader;
}

std::string ShaderCache::getShaderPath() const
{
    std::string shaderPath;
    switch (bgfx::getRendererType())
    {
    case bgfx::RendererType::Direct3D9:
        shaderPath = "shader/dx9/";
        break;
    case bgfx::RendererType::Direct3D11:
    case bgfx::RendererType::Direct3D12:
        shaderPath = "shader/dx11/";
        break;
    case bgfx::RendererType::Metal:
        shaderPath = "shader/metal/";
        break;
    case bgfx::RendererType::OpenGL:
        shaderPath = "shader/glsl/";
        break;
    case bgfx::RendererType::OpenGLES:
        shaderPath = "shader/essl/";
        break;
    case bgfx::RendererType::Vulkan:
        shaderPath = "shader/spirv/";
        break;
    default:
        break;
    }
    return shaderPath;
}

Shader* ShaderCache::load(String filename)
{
    auto it = shaders_.find(filename);
    if (it != shaders_.end())
    {
        return it->second;
    }
    std::string shaderFile = FileUtils::getInstance()->fullPathForFilename(getShaderPath() + filename);
    const bgfx::Memory* mem = FileUtils::getInstance()->getDataFromFileBX(shaderFile);
    bgfx::ShaderHandle handle = bgfx::createShader(mem);
    if (bgfx::isValid(handle))
    {
        Shader* shader = Shader::create(handle);
        shaders_[filename] = shader;
        return shader;
    }
    else
    {
        CCLOG("Failed to load shader \"%s\".", shaderFile.c_str());
        return nullptr;
    }
}

void ShaderCache::loadAsync(String filename, const std::function<void(Shader*)>& handler)
{
    
}

bool ShaderCache::unload(Shader* shader)
{
    for (const auto& it : shaders_)
    {
        if (it.second == shader)
        {
            shaders_.erase(shaders_.find(it.first));
            return true;
        }
    }
    return false;
}

bool ShaderCache::unload(String filename)
{
    std::string fullName = FileUtils::getInstance()->fullPathForFilename(getShaderPath() + filename);
    auto it = shaders_.find(fullName);
    if (it != shaders_.end())
    {
        shaders_.erase(it);
        return true;
    }
    return false;
}

bool ShaderCache::unload()
{
    if (shaders_.empty())
    {
        return false;
    }
    shaders_.clear();
    return true;
}

void ShaderCache::removeUnused()
{
    std::vector<std::unordered_map<std::string, SmartPtr<Shader>>::iterator> targets;
    for (auto it = shaders_.begin(); it != shaders_.end(); ++it)
    {
        if (it->second->getReferenceCount() == 1)
        {
            targets.push_back(it);
        }
    }
    for (const auto& it : targets)
    {
        shaders_.erase(it);
    }
}
NS_CC_END