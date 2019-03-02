#include "scripting/js-bindings/manual/jsb_cocos2dx_audioengine_manual.hpp"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "scripting/js-bindings/manual/jsb_conversions.hpp"
#include "scripting/js-bindings/manual/jsb_global.h"
#include "audio/Sound.h"
#include "audio/SoundCache.h"


se::Object* __jsb_cocos2d_AudioEngine_proto = nullptr;
se::Class* __jsb_cocos2d_AudioEngine_class = nullptr;

static bool js_cocos2dx_audioengine_AudioEngine_lazyInit(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        cocos2d::Audio* audio = &SharedAudio;
        bool result = audio != nullptr;
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_lazyInit : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_lazyInit)

static bool js_cocos2dx_audioengine_AudioEngine_setCurrentTime(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        int arg0 = 0;
        float arg1 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        ok &= seval_to_float(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_setCurrentTime : Error processing arguments");
        SharedAudio.getSoLoud().seek(arg0, arg1);
        ok &= boolean_to_seval(true, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_setCurrentTime : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_setCurrentTime)

static bool js_cocos2dx_audioengine_AudioEngine_getVolume(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        int arg0 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_getVolume : Error processing arguments");
        float result = SharedAudio.getSoLoud().getVolume(arg0);
        ok &= float_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_getVolume : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_getVolume)

static bool js_cocos2dx_audioengine_AudioEngine_uncache(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_uncache : Error processing arguments");
        SharedSoundCache.unload(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_uncache)

static bool js_cocos2dx_audioengine_AudioEngine_resumeAll(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        SharedAudio.setPauseAll(false);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_resumeAll)

static bool js_cocos2dx_audioengine_AudioEngine_stopAll(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        SharedAudio.getSoLoud().stopAll();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_stopAll)

static bool js_cocos2dx_audioengine_AudioEngine_pause(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        int arg0 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_pause : Error processing arguments");
        SharedAudio.setPause(arg0, true);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_pause)

static bool js_cocos2dx_audioengine_AudioEngine_end(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_end)

static bool js_cocos2dx_audioengine_AudioEngine_getMaxAudioInstance(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        int result = SharedAudio.getSoLoud().getMaxActiveVoiceCount();
        ok &= int32_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_getMaxAudioInstance : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_getMaxAudioInstance)

static bool js_cocos2dx_audioengine_AudioEngine_isEnabled(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        bool result = true;// cocos2d::experimental::AudioEngine::isEnabled();
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_isEnabled : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_isEnabled)

static bool js_cocos2dx_audioengine_AudioEngine_getCurrentTime(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        int arg0 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_getCurrentTime : Error processing arguments");
        float result = SharedAudio.getSoLoud().getStreamTime(arg0);
        ok &= float_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_getCurrentTime : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_getCurrentTime)

static bool js_cocos2dx_audioengine_AudioEngine_setMaxAudioInstance(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        int arg0 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_setMaxAudioInstance : Error processing arguments");
        bool result = SharedAudio.getSoLoud().setMaxActiveVoiceCount(arg0);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_setMaxAudioInstance : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_setMaxAudioInstance)

static bool js_cocos2dx_audioengine_AudioEngine_isLoop(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        int arg0 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_isLoop : Error processing arguments");
        bool result = SharedAudio.getSoLoud().getLooping(arg0);
        ok &= boolean_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_isLoop : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_isLoop)

static bool js_cocos2dx_audioengine_AudioEngine_pauseAll(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        SharedAudio.setPauseAll(true);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_pauseAll)

static bool js_cocos2dx_audioengine_AudioEngine_uncacheAll(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        SharedSoundCache.unload();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_uncacheAll)

static bool js_cocos2dx_audioengine_AudioEngine_setVolume(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        int arg0 = 0;
        float arg1 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        ok &= seval_to_float(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_setVolume : Error processing arguments");
        SharedAudio.getSoLoud().setVolume(arg0, arg1);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_setVolume)

static bool js_cocos2dx_audioengine_AudioEngine_preload(se::State& s)
{
    CC_UNUSED bool ok = true;
    const auto& args = s.args();
    size_t argc = args.size();
    do {
        if (argc == 2) {
            std::string arg0;
            ok &= seval_to_std_string(args[0], &arg0);
            if (!ok) { ok = true; break; }
            std::function<void(bool)> arg1;
            do {
                if (args[1].isObject() && args[1].toObject()->isFunction())
                {
                    se::Value jsThis(s.thisObject());
                    se::Value jsFunc(args[1]);
                    jsFunc.toObject()->root();
                    auto lambda = [=](bool larg0) -> void {
                        se::ScriptEngine::getInstance()->clearException();
                        se::AutoHandleScope hs;

                        CC_UNUSED bool ok = true;
                        se::ValueArray args;
                        args.resize(1);
                        ok &= boolean_to_seval(larg0, &args[0]);
                        se::Value rval;
                        se::Object* thisObj = jsThis.isObject() ? jsThis.toObject() : nullptr;
                        se::Object* funcObj = jsFunc.toObject();
                        bool succeed = funcObj->call(args, thisObj, &rval);
                        if (!succeed) {
                            se::ScriptEngine::getInstance()->clearException();
                        }
                    };
                    arg1 = lambda;
                }
                else
                {
                    arg1 = nullptr;
                }
            } while (false)
                ;
            if (!ok) { ok = true; break; }
            //cocos2d::experimental::AudioEngine::preload(arg0, arg1);
            return true;
        }
    } while (false);
    do {
        if (argc == 1) {
            std::string arg0;
            ok &= seval_to_std_string(args[0], &arg0);
            if (!ok) { ok = true; break; }
            //cocos2d::experimental::AudioEngine::preload(arg0);
            return true;
        }
    } while (false);
    SE_REPORT_ERROR("wrong number of arguments: %d", (int)argc);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_preload)

static bool js_cocos2dx_audioengine_AudioEngine_setEnabled(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        bool arg0;
        ok &= seval_to_boolean(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_setEnabled : Error processing arguments");
        //no impl//cocos2d::experimental::AudioEngine::setEnabled(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_setEnabled)

static bool js_cocos2dx_audioengine_AudioEngine_play2d(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        std::string arg0;
        ok &= seval_to_std_string(args[0], &arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_play2d : Error processing arguments");
        int result = SharedAudio.play(arg0);
        ok &= int32_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_play2d : Error processing arguments");
        return true;
    }
    if (argc == 2) {
        std::string arg0;
        bool arg1;
        ok &= seval_to_std_string(args[0], &arg0);
        ok &= seval_to_boolean(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_play2d : Error processing arguments");
        int result = SharedAudio.play(arg0, arg1);
        ok &= int32_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_play2d : Error processing arguments");
        return true;
    }
    if (argc == 3) {
        std::string arg0;
        bool arg1;
        float arg2 = 0;
        ok &= seval_to_std_string(args[0], &arg0);
        ok &= seval_to_boolean(args[1], &arg1);
        ok &= seval_to_float(args[2], &arg2);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_play2d : Error processing arguments");
        int result = SharedAudio.play(arg0, arg1, arg2);
        ok &= int32_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_play2d : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 4);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_play2d)

static bool js_cocos2dx_audioengine_AudioEngine_getState(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        int arg0 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_getState : Error processing arguments");
        int result = 0;// (int)cocos2d::experimental::AudioEngine::getState(arg0);
        ok &= int32_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_getState : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_getState)

static bool js_cocos2dx_audioengine_AudioEngine_resume(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        int arg0 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_resume : Error processing arguments");
        SharedAudio.setPause(arg0, false);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_resume)

static bool js_cocos2dx_audioengine_AudioEngine_stop(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        int arg0 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_stop : Error processing arguments");
        SharedAudio.stop(arg0);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_stop)

static bool js_cocos2dx_audioengine_AudioEngine_getDuration(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        int arg0 = 0;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_getDuration : Error processing arguments");
        float result = 0.0f;// SharedAudio.getSoLoud().getStreamTime(arg0);
        ok &= float_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_getDuration : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_getDuration)

static bool js_cocos2dx_audioengine_AudioEngine_setLoop(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        int arg0 = 0;
        bool arg1;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        ok &= seval_to_boolean(args[1], &arg1);
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_setLoop : Error processing arguments");
        SharedAudio.setLoop(arg0, arg1);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_setLoop)


static bool js_cocos2dx_audioengine_AudioEngine_setFinishCallback(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 2) {
        int arg0 = 0;
        std::function<void(int, const std::basic_string<char> &)> arg1;
        ok &= seval_to_int32(args[0], (int32_t*)&arg0);
        do {
            if (args[1].isObject() && args[1].toObject()->isFunction())
            {
                se::Value jsThis(s.thisObject());
                se::Value jsFunc(args[1]);
                jsFunc.toObject()->root();
                auto lambda = [=](int larg0, const std::basic_string<char> & larg1) -> void {
                    se::ScriptEngine::getInstance()->clearException();
                    se::AutoHandleScope hs;

                    CC_UNUSED bool ok = true;
                    se::ValueArray args;
                    args.resize(2);
                    ok &= int32_to_seval(larg0, &args[0]);
                    ok &= std_string_to_seval(larg1, &args[1]);
                    se::Value rval;
                    se::Object* thisObj = jsThis.isObject() ? jsThis.toObject() : nullptr;
                    se::Object* funcObj = jsFunc.toObject();
                    bool succeed = funcObj->call(args, thisObj, &rval);
                    if (!succeed) {
                        se::ScriptEngine::getInstance()->clearException();
                    }
                };
                arg1 = lambda;
            }
            else
            {
                arg1 = nullptr;
            }
        } while (false)
            ;
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_setFinishCallback : Error processing arguments");
        //cocos2d::experimental::AudioEngine::setFinishCallback(arg0, arg1);
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_setFinishCallback)

static bool js_cocos2dx_audioengine_AudioEngine_getPlayingAudioCount(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        int result = SharedAudio.getPlayingAudioCount();
        ok &= int32_to_seval(result, &s.rval());
        SE_PRECONDITION2(ok, false, "js_cocos2dx_audioengine_AudioEngine_getPlayingAudioCount : Error processing arguments");
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_cocos2dx_audioengine_AudioEngine_getPlayingAudioCount)




bool js_register_cocos2dx_audioengine_AudioEngine(se::Object* obj)
{
    auto cls = se::Class::create("AudioEngine", obj, nullptr, nullptr);

    cls->defineStaticFunction("lazyInit", _SE(js_cocos2dx_audioengine_AudioEngine_lazyInit));
    cls->defineStaticFunction("setCurrentTime", _SE(js_cocos2dx_audioengine_AudioEngine_setCurrentTime));
    cls->defineStaticFunction("getVolume", _SE(js_cocos2dx_audioengine_AudioEngine_getVolume));
    cls->defineStaticFunction("uncache", _SE(js_cocos2dx_audioengine_AudioEngine_uncache));
    cls->defineStaticFunction("resumeAll", _SE(js_cocos2dx_audioengine_AudioEngine_resumeAll));
    cls->defineStaticFunction("stopAll", _SE(js_cocos2dx_audioengine_AudioEngine_stopAll));
    cls->defineStaticFunction("pause", _SE(js_cocos2dx_audioengine_AudioEngine_pause));
    cls->defineStaticFunction("end", _SE(js_cocos2dx_audioengine_AudioEngine_end));
    cls->defineStaticFunction("getMaxAudioInstance", _SE(js_cocos2dx_audioengine_AudioEngine_getMaxAudioInstance));
    cls->defineStaticFunction("isEnabled", _SE(js_cocos2dx_audioengine_AudioEngine_isEnabled));
    cls->defineStaticFunction("getCurrentTime", _SE(js_cocos2dx_audioengine_AudioEngine_getCurrentTime));
    cls->defineStaticFunction("setMaxAudioInstance", _SE(js_cocos2dx_audioengine_AudioEngine_setMaxAudioInstance));
    cls->defineStaticFunction("isLoop", _SE(js_cocos2dx_audioengine_AudioEngine_isLoop));
    cls->defineStaticFunction("pauseAll", _SE(js_cocos2dx_audioengine_AudioEngine_pauseAll));
    cls->defineStaticFunction("uncacheAll", _SE(js_cocos2dx_audioengine_AudioEngine_uncacheAll));
    cls->defineStaticFunction("setVolume", _SE(js_cocos2dx_audioengine_AudioEngine_setVolume));
    cls->defineStaticFunction("preload", _SE(js_cocos2dx_audioengine_AudioEngine_preload));
    cls->defineStaticFunction("setEnabled", _SE(js_cocos2dx_audioengine_AudioEngine_setEnabled));
    cls->defineStaticFunction("play2d", _SE(js_cocos2dx_audioengine_AudioEngine_play2d));
    cls->defineStaticFunction("getState", _SE(js_cocos2dx_audioengine_AudioEngine_getState));
    cls->defineStaticFunction("resume", _SE(js_cocos2dx_audioengine_AudioEngine_resume));
    cls->defineStaticFunction("stop", _SE(js_cocos2dx_audioengine_AudioEngine_stop));
    cls->defineStaticFunction("getDuration", _SE(js_cocos2dx_audioengine_AudioEngine_getDuration));
    cls->defineStaticFunction("setLoop", _SE(js_cocos2dx_audioengine_AudioEngine_setLoop));
    cls->defineStaticFunction("setFinishCallback", _SE(js_cocos2dx_audioengine_AudioEngine_setFinishCallback));
    cls->defineStaticFunction("getPlayingAudioCount", _SE(js_cocos2dx_audioengine_AudioEngine_getPlayingAudioCount));
    cls->install();
    JSBClassType::registerClass<cocos2d::Audio>(cls);

    __jsb_cocos2d_AudioEngine_proto = cls->getProto();
    __jsb_cocos2d_AudioEngine_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}

bool register_all_cocos2dx_audioengine(se::Object* obj)
{
    // Get the ns
    se::Value nsVal;
    if (!obj->getProperty("jsb", &nsVal))
    {
        se::HandleObject jsobj(se::Object::createPlainObject());
        nsVal.setObject(jsobj);
        obj->setProperty("jsb", nsVal);
    }
    se::Object* ns = nsVal.toObject();

    js_register_cocos2dx_audioengine_AudioEngine(ns);
    return true;
}

#endif //#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
