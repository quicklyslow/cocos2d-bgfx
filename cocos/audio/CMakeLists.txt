
if(WINDOWS)

    set(COCOS_AUDIO_PLATFORM_SRC
        audio/soloud/src/backend/winmm/soloud_winmm.cpp
        )

elseif(ANDROID)

    set(COCOS_AUDIO_PLATFORM_SRC
        audio/soloud/src/backend/opensles/soloud_opensles.cpp
        )

elseif(APPLE)
    # common
    set(COCOS_AUDIO_PLATFORM_SRC
        audio/soloud/src/backend/coreaudio/soloud_coreaudio.cpp
        )
    # split it in _C and non C
    # because C files needs to be compiled with C compiler and not C++
    # compiler
    # if(MACOSX)
    #     set(COCOS_AUDIO_PLATFORM_HEADER
    #         ${COCOS_AUDIO_PLATFORM_HEADER}
    #         audio/mac/CDConfig.h
    #         audio/mac/SimpleAudioEngine_objc.h
    #         audio/mac/CDOpenALSupport.h
    #         audio/mac/CDXMacOSXSupport.h
    #         audio/mac/CDAudioManager.h
    #         audio/mac/CocosDenshion.h
    #         )
    #     set(COCOS_AUDIO_PLATFORM_SRC_C
    #         audio/mac/CDAudioManager.m
    #         audio/mac/CDOpenALSupport.m
    #         audio/mac/CocosDenshion.m
    #         audio/mac/SimpleAudioEngine_objc.m
    #         )
    #     set(COCOS_AUDIO_PLATFORM_SRC
    #         ${COCOS_AUDIO_PLATFORM_SRC_C}
    #         ${COCOS_AUDIO_PLATFORM_SRC}
    #         audio/mac/SimpleAudioEngine.mm
    #         audio/mac/CDXMacOSXSupport.mm
    #         )
    # elseif(IOS)
    #     set(COCOS_AUDIO_PLATFORM_HEADER
    #         ${COCOS_AUDIO_PLATFORM_HEADER}
    #         audio/ios/CDConfig.h
    #         audio/ios/SimpleAudioEngine_objc.h
    #         audio/ios/CDOpenALSupport.h
    #         audio/ios/CDAudioManager.h
    #         audio/ios/CocosDenshion.h
    #         )
    #     set(COCOS_AUDIO_PLATFORM_SRC_C
    #         audio/ios/CDAudioManager.m
    #         audio/ios/CDOpenALSupport.m
    #         audio/ios/CocosDenshion.m
    #         audio/ios/SimpleAudioEngine_objc.m
    #         )
    #     set(COCOS_AUDIO_PLATFORM_SRC
    #         ${COCOS_AUDIO_PLATFORM_SRC_C}
    #         ${COCOS_AUDIO_PLATFORM_SRC}
    #         audio/ios/SimpleAudioEngine.mm
    #         )
    # endif()

    set_source_files_properties(
            ${COCOS_AUDIO_PLATFORM_SRC_C}
            PROPERTIES LANGUAGE C
    )
endif()

set(COCOS_AUDIO_HEADER
    audio/Sound.h
    audio/SoundCache.h
    audio/soloud/include/soloud.h
    audio/soloud/include/soloud_audiosource.h
    audio/soloud/include/soloud_bassboostfilter.h
    audio/soloud/include/soloud_biquadresonantfilter.h
    audio/soloud/include/soloud_bus.h
    audio/soloud/include/soloud_c.h
    audio/soloud/include/soloud_dcremovalfilter.h
    audio/soloud/include/soloud_echofilter.h
    audio/soloud/include/soloud_error.h
    audio/soloud/include/soloud_fader.h
    audio/soloud/include/soloud_fft.h
    audio/soloud/include/soloud_fftfilter.h
    audio/soloud/include/soloud_file.h
    audio/soloud/include/soloud_file_hack_off.h
    audio/soloud/include/soloud_file_hack_on.h
    audio/soloud/include/soloud_filter.h
    audio/soloud/include/soloud_flangerfilter.h
    audio/soloud/include/soloud_internal.h
    audio/soloud/include/soloud_lofifilter.h
    audio/soloud/include/soloud_modplug.h
    audio/soloud/include/soloud_monotone.h
    audio/soloud/include/soloud_sfxr.h
    audio/soloud/include/soloud_speech.h
    audio/soloud/include/soloud_tedsid.h
    audio/soloud/include/soloud_thread.h
    audio/soloud/include/soloud_vic.h
    audio/soloud/include/soloud_wav.h
    audio/soloud/include/soloud_wavstream.h
    )
set(COCOS_AUDIO_SRC
    audio/Sound.cpp
    audio/SoundCache.cpp
    audio/soloud/src/audiosource/modplug/soloud_modplug.cpp
    audio/soloud/src/audiosource/monotone/soloud_monotone.cpp
    audio/soloud/src/audiosource/sfxr/soloud_sfxr.cpp
    audio/soloud/src/audiosource/speech/darray.cpp
    audio/soloud/src/audiosource/speech/klatt.cpp
    audio/soloud/src/audiosource/speech/resonator.cpp
    audio/soloud/src/audiosource/speech/soloud_speech.cpp
    audio/soloud/src/audiosource/speech/tts.cpp
    audio/soloud/src/audiosource/tedsid/sid.cpp
    audio/soloud/src/audiosource/tedsid/soloud_tedsid.cpp
    audio/soloud/src/audiosource/tedsid/ted.cpp
    audio/soloud/src/audiosource/vic/soloud_vic.cpp
    audio/soloud/src/audiosource/wav/soloud_wav.cpp
    audio/soloud/src/audiosource/wav/soloud_wavstream.cpp
    audio/soloud/src/audiosource/wav/stb_vorbis.c
    audio/soloud/src/core/soloud.cpp
    audio/soloud/src/core/soloud_audiosource.cpp
    audio/soloud/src/core/soloud_bus.cpp
    audio/soloud/src/core/soloud_core_3d.cpp
    audio/soloud/src/core/soloud_core_basicops.cpp
    audio/soloud/src/core/soloud_core_faderops.cpp
    audio/soloud/src/core/soloud_core_filterops.cpp
    audio/soloud/src/core/soloud_core_getters.cpp
    audio/soloud/src/core/soloud_core_setters.cpp
    audio/soloud/src/core/soloud_core_voicegroup.cpp
    audio/soloud/src/core/soloud_core_voiceops.cpp
    audio/soloud/src/core/soloud_fader.cpp
    audio/soloud/src/core/soloud_fft.cpp
    audio/soloud/src/core/soloud_fft_lut.cpp
    audio/soloud/src/core/soloud_file.cpp
    audio/soloud/src/core/soloud_filter.cpp
    audio/soloud/src/core/soloud_thread.cpp
    audio/soloud/src/filter/soloud_bassboostfilter.cpp
    audio/soloud/src/filter/soloud_biquadresonantfilter.cpp
    audio/soloud/src/filter/soloud_dcremovalfilter.cpp
    audio/soloud/src/filter/soloud_echofilter.cpp
    audio/soloud/src/filter/soloud_fftfilter.cpp
    audio/soloud/src/filter/soloud_flangerfilter.cpp
    audio/soloud/src/filter/soloud_lofifilter.cpp
    ${COCOS_AUDIO_PLATFORM_SRC}
        )
