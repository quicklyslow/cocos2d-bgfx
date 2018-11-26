#pragma once

NS_CC_BEGIN

class Shader : public Ref
{
    PROPERTY_READONLY(bgfx::ShaderHandle, Handle);
    virtual ~Shader();
    CREATE_FUNC(Shader);
protected:
    Shader(bgfx::ShaderHandle handle);
private:
    bgfx::ShaderHandle handle_;
};

class ShaderCache
{
public:
    ~ShaderCache() {}
    void update(String name, Shader* shader);
    Shader* load(String filename);
    void loadAsync(String filename, const std::function<void(Shader*)>& handler);
    bool unload(Shader* shader);
    bool unload(String filename);
    bool unload();
    void removeUnused();
protected:
    ShaderCache();
    std::string getShaderPath() const;
private:
    std::unordered_map<std::string, SmartPtr<Shader>> shaders_;
    SINGLETON_REF(ShaderCache, BGFXCocos);
};

#define SharedShaderCache \
    cocos2d::Singleton<cocos2d::ShaderCache>::shared()

NS_CC_END