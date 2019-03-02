#pragma once

#include "base/CCData.h"
#include "soloud/include/soloud_wav.h"
#include "soloud/include/soloud_wavstream.h"

NS_CC_BEGIN

class EventCustom;
class EventListener;

class SoundFile : public Ref
{
public:
    PROPERTY_READONLY_CALL(SoLoud::Wav&, Wav);
    virtual bool init() override;
    CREATE_FUNC(SoundFile);
protected:
    SoundFile(Data&& data);
private:
    Data data_;
    SoLoud::Wav wav_;
    COCOS_TYPE_OVERRIDE(SoundFile);
};

class SoundStream : public Ref
{
public:
    PROPERTY_READONLY_CALL(SoLoud::WavStream&, Stream);
    virtual bool init() override;
    CREATE_FUNC(SoundStream);
protected:
    SoundStream(Data&& data);
private:
    Data data_;
    SoLoud::WavStream stream_;
    COCOS_TYPE_OVERRIDE(SoundStream);
};

class Audio
{
public:
    PROPERTY_READONLY_CALL(SoLoud::Soloud&, SoLoud);
    ~Audio();
    bool init();
    uint32_t play(std::string& filename, bool loop = false, float volume = -1.0f);
    void stop(uint32_t handle);
    void playStream(std::string& filename, bool loop = false, float crossFadeTime = 0.0f);
    void stopStream(float fadeTime = 0.0f);
    int32_t getPlayingAudioCount();
    void setLoop(uint32_t handle, bool loop);
    void setPause(uint32_t handle, bool pause);
    void setPauseAll(bool pause);
protected:
    Audio();
private:
    void onEnterBackground(EventCustom* event);
    void onEnterForeground(EventCustom* event);
private:
    uint32_t currentVoice_;
    SmartPtr<SoundStream> lastStream_;
    SmartPtr<SoundStream> currentStream_;
    SoLoud::Soloud soloud_;

    EventListener* onPauseListener_;
    EventListener* onResumeListener_;
    SINGLETON_REF(Audio, Director);
};

#define SharedAudio \
    cocos2d::Singleton<cocos2d::Audio>::shared()

NS_CC_END
