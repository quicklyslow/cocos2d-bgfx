#include "ccHeader.h"
#include "SoundCache.h"
#include "Sound.h"
#include "platform/CCFileUtils.h"

NS_CC_BEGIN


SoundFile* SoundCache::get(const std::string& filename)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
    auto it = soundFiles_.find(fullPath);
    if (it != soundFiles_.end())
    {
        return it->second;
    }
    return nullptr;
}

SoundFile* SoundCache::load(const std::string& filename)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
    auto it = soundFiles_.find(fullPath);
    if (it != soundFiles_.end())
    {
        return it->second;
    }
    Data data = FileUtils::getInstance()->getDataFromFile(fullPath);
    if (!data.isNull())
    {
        SoundFile* soundFile = SoundFile::create(std::move(data));
        if (soundFile)
        {
            soundFiles_.emplace(fullPath, soundFile);
            return soundFile;
        }
    }
    return nullptr;
}

bool SoundCache::unload(SoundFile* soundFile)
{
    for (const auto& it : soundFiles_)
    {
        if (it.second == soundFile)
        {
            soundFiles_.erase(soundFiles_.find(it.first));
            return true;
        }
    }
    return false;
}

bool SoundCache::unload(std::string& filename)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
    auto it = soundFiles_.find(fullPath);
    if (it != soundFiles_.end())
    {
        soundFiles_.erase(it);
        return true;
    }
    return false;
}

bool SoundCache::unload()
{
    if (soundFiles_.empty())
    {
        return false;
    }
    soundFiles_.clear();
    return true;
}

void SoundCache::removeUnused()
{
    std::vector<std::unordered_map<std::string, SmartPtr<SoundFile>>::iterator> targets;
    for (auto it = soundFiles_.begin(); it != soundFiles_.end(); ++it)
    {
        if (it->second->getReferenceCount() == 1)
        {
            targets.push_back(it);
        }
    }
    for (const auto& it : targets)
    {
        soundFiles_.erase(it);
    }
}

NS_CC_END