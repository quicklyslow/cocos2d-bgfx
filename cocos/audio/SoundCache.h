#pragma once

NS_CC_BEGIN

class SoundFile;

class CC_DLL SoundCache
{
public:
    SoundFile* get(const std::string& filename);
    SoundFile* load(const std::string& filename);
    bool unload(SoundFile* soundFile);
    bool unload(std::string& filename);
    bool unload();
    void removeUnused();
protected:
    SoundCache() {}
private:
    std::unordered_map<std::string, SmartPtr<SoundFile>> soundFiles_;
    SINGLETON_REF(SoundCache, Audio);
};

#define SharedSoundCache \
    cocos2d::Singleton<cocos2d::SoundCache>::shared()
NS_CC_END