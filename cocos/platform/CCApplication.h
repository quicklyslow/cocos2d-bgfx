/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#ifndef __CC_APPLICATION_WIN32_H__
#define __CC_APPLICATION_WIN32_H__

#include "platform/CCPlatformConfig.h"
#include "platform/CCApplicationProtocol.h"
#include "platform/CCCommon.h"
#include "base/EventQueue.h"

NS_CC_BEGIN

class GLView;
class Rect;

class CC_DLL Application
{
public:
    /** Since WINDOWS and ANDROID are defined as macros, we could not just use these keywords in enumeration(Platform).
     *  Therefore, 'OS_' prefix is added to avoid conflicts with the definitions of system macros.
     */
    enum class Platform
    {
        OS_WINDOWS,     /**< Windows */
        OS_LINUX,       /**< Linux */
        OS_MAC,         /**< Mac OS X*/
        OS_ANDROID,     /**< Android */
        OS_IPHONE,      /**< iPhone */
        OS_IPAD,        /**< iPad */
        OS_BLACKBERRY,  /**< BlackBerry */
        OS_NACL,        /**< Native Client in Chrome */
        OS_EMSCRIPTEN,  /**< Emscripten */
        OS_TIZEN,       /**< Tizen */
        OS_WINRT,       /**< Windows Runtime Applications */
        OS_WP8          /**< Windows Phone 8 Applications */
    };

    PROPERTY_READONLY(double, LastTime);
    PROPERTY_READONLY(double, DeltaTime);
    PROPERTY_READONLY(double, ElapsedTime);
    PROPERTY_READONLY(double, CurrentTime);
    PROPERTY_READONLY(double, CPUTime);
    PROPERTY_READONLY(double, TotalTime);
    PROPERTY(uint32_t, MaxFPS);
    PROPERTY(uint32_t, MinFPS);
#if BX_PLATFORM_WINDOWS
    inline void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }
    inline void operator delete(void* p)
    {
        _mm_free(p);
    }
#endif
    /**
     * @js NA
     * @lua NA
     */
    virtual ~Application();

    /**
    @brief    Run the message loop.
    */
    int run();
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    void iOSEventLoop();
#endif

    static int mainLogic(bx::Thread* thread, void* userData);

    void setStartupScriptFilename(const std::string& startupScriptFile);

    const std::string& getStartupScriptFilename(void)
    {
        return _startupScriptFilename;
    }

    template<typename... Args>
    void postEventToLogic(String name, const Args& ...args)
    {
        logicEvent_.post(name, args...);
    }

    void invokeInRenderer(const std::function<void()>& func);

    void invokeInLogic(const std::function<void()>& func);

    void setAppDelegate(ApplicationProtocol* app);

    /* override functions */
    void setAnimationInterval(float interval);

    LanguageType getCurrentLanguage();

    const char * getCurrentLanguageCode();

    /**
     @brief Get target platform
     */
    Platform getTargetPlatform();
    /**
    @brief Get application version
    */
    std::string getVersion();
    /**
     @brief Open url in default browser
     @param String with url to open.
     @return true if the resource located by the URL was successfully opened; otherwise false.
     */
    bool openURL(const std::string &url);

	const char* getMacAddress(void);

	const char* getAdvertisingIdentifier(void);

	const char* getOpenUDID(void);

	const char* getIMEI(void);

	const char* getUDID(void);

	int getAvailableMemory(void);

    bool applicationDidFinishLaunching() { return _appDelegate->applicationDidFinishLaunching(); }

    /**
    * @brief  This function will be called when the application enters background.
    * @js NA
    * @lua NA
    */
    void applicationDidEnterBackground() { _appDelegate->applicationDidEnterBackground(); }

    /**
    * @brief  This function will be called when the application enters foreground.
    * @js NA
    * @lua NA
    */
    void applicationWillEnterForeground() { _appDelegate->applicationWillEnterForeground(); }
protected:
    Application();
    void updateDeltaTime();
    void makeTimeNow();
    bgfx::RenderFrame::Enum renderFrame();
protected:
#if BX_PLATFORM_WINDOWS
    HINSTANCE           _instance;
    HACCEL              _accelTable;
    LARGE_INTEGER       _animationInterval;
#endif
    std::string         _resourceRootPath;
    std::string         _startupScriptFilename;
	std::string			m_macAddress;
	std::string         m_advertisingIdentifier;
	std::string			m_openUDID;
	std::string			m_imei;
	std::string         m_udid;
    uint32_t            frame_;

    bx::Thread          _logicThread;

    cocos2d::GLView*     glView_;
private:
    uint32_t _maxFPS;
    uint32_t _minFPS;
    bool _fpsLimited;
    const double _frequency;
    double _lastTime;
    double _deltaTime;
    double _cpuTime;
    double _totalTime;

    ApplicationProtocol* _appDelegate;

    EventQueue logicEvent_;
    EventQueue renderEvent_;

    SINGLETON_REF(Application, AsyncLogThread);
};

#define SharedApplication \
    cocos2d::Singleton<cocos2d::Application>::shared()

class BGFXCocos
{
public:
    bool init();
    virtual ~BGFXCocos();
    SINGLETON_REF(BGFXCocos);
};

#define SharedBGFX \
    cocos2d::Singleton<cocos2d::BGFXCocos>::shared()

NS_CC_END


#endif    // __CC_APPLICATION_WIN32_H__
