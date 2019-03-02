LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := soloud_static

LOCAL_MODULE_FILENAME := libsoloud

LOCAL_SRC_FILES :=  src/audiosource/modplug/soloud_modplug.cpp \
                    src/audiosource/monotone/soloud_monotone.cpp \
                    src/audiosource/sfxr/soloud_sfxr.cpp \
                    src/audiosource/speech/darray.cpp \
                    src/audiosource/speech/klatt.cpp \
                    src/audiosource/speech/resonator.cpp \
                    src/audiosource/speech/soloud_speech.cpp \
                    src/audiosource/speech/tts.cpp \
                    src/audiosource/tedsid/sid.cpp \
                    src/audiosource/tedsid/soloud_tedsid.cpp \
                    src/audiosource/tedsid/ted.cpp \
                    src/audiosource/vic/soloud_vic.cpp \
                    src/audiosource/wav/soloud_wav.cpp \
                    src/audiosource/wav/soloud_wavstream.cpp \
                    src/audiosource/wav/stb_vorbis.c \
                    src/core/soloud.cpp \
                    src/core/soloud_audiosource.cpp \
                    src/core/soloud_bus.cpp \
                    src/core/soloud_core_3d.cpp \
                    src/core/soloud_core_basicops.cpp \
                    src/core/soloud_core_faderops.cpp \
                    src/core/soloud_core_filterops.cpp \
                    src/core/soloud_core_getters.cpp \
                    src/core/soloud_core_setters.cpp \
                    src/core/soloud_core_voicegroup.cpp \
                    src/core/soloud_core_voiceops.cpp \
                    src/core/soloud_fader.cpp \
                    src/core/soloud_fft.cpp \
                    src/core/soloud_fft_lut.cpp \
                    src/core/soloud_file.cpp \
                    src/core/soloud_filter.cpp \
                    src/core/soloud_thread.cpp \
                    src/filter/soloud_bassboostfilter.cpp \
                    src/filter/soloud_biquadresonantfilter.cpp \
                    src/filter/soloud_dcremovalfilter.cpp \
                    src/filter/soloud_echofilter.cpp \
                    src/filter/soloud_fftfilter.cpp \
                    src/filter/soloud_flangerfilter.cpp \
                    src/filter/soloud_lofifilter.cpp \
                    src/backend/opensles/soloud_opensles.cpp \
                    ../sound.cpp \
                    ../SoundCache.cpp \

LOCAL_CFLAGS := -DMODPLUG_STATIC=1 -DWITH_MODPLUG=1 -DWITH_OPENSLES=1
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_EXPORT_LDLIBS := -lOpenSLES

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/../.. \

include $(BUILD_STATIC_LIBRARY)
