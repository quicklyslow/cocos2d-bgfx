#include "ccHeader.h"
#include "Sound.h"
#include "SoundCache.h"
#include "base/CCConsole.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"
#include "base/CCEventType.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventDispatcher.h"
#include "platform/CCFileUtils.h"


static const int AUDIOFOCUS_GAIN = 0;
static const int AUDIOFOCUS_LOST = 1;
static const int AUDIOFOCUS_LOST_TRANSIENT = 2;
static const int AUDIOFOCUS_LOST_TRANSIENT_CAN_DUCK = 3;

static int __currentAudioFocus = AUDIOFOCUS_GAIN;

NS_CC_BEGIN


SoundFile::SoundFile(Data&& data)
    :data_(std::move(data))
{

}

bool SoundFile::init()
{
    SoLoud::result result = wav_.loadMem(data_.getBytes(), static_cast<uint32_t>(data_.getSize()), false, false);
    data_.clear();
    if (result)
    {
        CCLOGERROR("failed to load sound file due to reason : %s", SharedAudio.getSoLoud().getErrorString(result));
        return false;
    }
    return true;
}

SoLoud::Wav& SoundFile::getWav()
{
    return wav_;
}

SoundStream::SoundStream(Data&& data)
    :data_(std::move(data))
{

}

bool SoundStream::init()
{
    SoLoud::result result = stream_.loadMem(data_.getBytes(), static_cast<uint32_t>(data_.getSize()), false, false);
    if (result)
    {
        CCLOGERROR("fail to load sound stream file due to reason: %s.", SharedAudio.getSoLoud().getErrorString(result));
        return false;
    }
    return true;
}

SoLoud::WavStream& SoundStream::getStream()
{
    return stream_;
}

Audio::Audio()
    :currentVoice_(0)
    , onPauseListener_(nullptr)
    , onResumeListener_(nullptr)
{

}

Audio::~Audio()
{
    soloud_.deinit();
    if (onPauseListener_ != nullptr)
    {
        SharedDirector.getEventDispatcher()->removeEventListener(onPauseListener_);
    }

    if (onResumeListener_ != nullptr)
    {
        SharedDirector.getEventDispatcher()->removeEventListener(onResumeListener_);
    }
}

bool Audio::init()
{
    SoLoud::result result = soloud_.init();
    if (result)
    {
        CCLOGERROR("fail to init soloud engine deal to reason: {}.", soloud_.getErrorString(result));
        return false;
    }
    onPauseListener_ = SharedDirector.getEventDispatcher()->addCustomEventListener(EVENT_COME_TO_BACKGROUND, CC_CALLBACK_1(Audio::onEnterBackground, this));

    onResumeListener_ = SharedDirector.getEventDispatcher()->addCustomEventListener(EVENT_COME_TO_FOREGROUND, CC_CALLBACK_1(Audio::onEnterForeground, this));
    return true;
}

void Audio::onEnterBackground(EventCustom* event)
{
    setPauseAll(true);
}

void Audio::onEnterForeground(EventCustom* event)
{
    setPauseAll(false);
}

SoLoud::Soloud& Audio::getSoLoud()
{
    return soloud_;
}

uint32_t Audio::play(std::string& filename, bool loop, float volume)
{
    SoundFile* file = SharedSoundCache.load(filename);
    if (file)
    {
        SoLoud::handle handle = soloud_.play(file->getWav(), volume);
        soloud_.setLooping(handle, loop);
        soloud_.setInaudibleBehavior(handle, true, true);
        return handle;
    }
    return 0;
}

void Audio::stop(uint32_t handle)
{
    soloud_.stop(handle);
}

void Audio::playStream(std::string& filename, bool loop, float crossFadeTime)
{
    if (lastStream_)
    {
        lastStream_->getStream().stop();
        lastStream_->release();
        lastStream_ = nullptr;
    }
    stopStream(crossFadeTime);
    Data data = FileUtils::getInstance()->getDataFromFile(filename);
    if (!data.isNull())
    {
        if (currentStream_)
        {
            currentStream_->getStream().stop();
        }
        currentStream_ = SoundStream::create(std::move(data));
        currentVoice_ = soloud_.play(currentStream_->getStream(), 0.0f);
        soloud_.setLooping(currentVoice_, loop);
        soloud_.setProtectVoice(currentVoice_, true);
        soloud_.fadeVolume(currentVoice_, 1.0f, crossFadeTime);
    }
}

void Audio::stopStream(float fadeTime)
{
    if (fadeTime > 0.0f)
    {
        if (currentVoice_ && soloud_.isValidVoiceHandle(currentVoice_))
        {
            soloud_.fadeVolume(currentVoice_, 0.0f, fadeTime);
            soloud_.scheduleStop(currentVoice_, fadeTime);
            lastStream_ = currentStream_;
            SharedDirector.getScheduler()->schedule([this](float t)
            {
                if (lastStream_)
                {
                    lastStream_->getStream().stop();
                    CC_SAFE_RELEASE(lastStream_);
                }
            }, currentStream_, fadeTime, false, "");
        }
    }
    else if (currentStream_)
    {
        currentStream_->getStream().stop();
    }
    currentVoice_ = 0;
    CC_SAFE_RELEASE(currentStream_);
}

int32_t Audio::getPlayingAudioCount()
{
    return soloud_.getActiveVoiceCount();
}

void Audio::setLoop(uint32_t handle, bool loop)
{
    soloud_.setLooping(handle, loop);
}

void Audio::setPause(uint32_t handle, bool pause)
{
    soloud_.setPause(handle, pause);
}

void Audio::setPauseAll(bool pause)
{
    soloud_.setPauseAll(pause);
}

NS_CC_END

// It's invoked from javaactivity-android.cpp
void cocos_audioengine_focus_change(int focusChange)
{
    if (focusChange < AUDIOFOCUS_GAIN || focusChange > AUDIOFOCUS_LOST_TRANSIENT_CAN_DUCK)
    {
        CCLOGERROR("cocos_audioengine_focus_change: unknown value: %d", focusChange);
        return;
    }
    CCLOG("cocos_audioengine_focus_change: %d", focusChange);
    __currentAudioFocus = focusChange;


    if (__currentAudioFocus == AUDIOFOCUS_GAIN)
    {
        SharedAudio.setPauseAll(false);
    }
    else
    {
        SharedAudio.setPauseAll(true);
    }
}