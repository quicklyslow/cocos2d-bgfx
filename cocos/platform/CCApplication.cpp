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


#include "ccHeader.h"
#include "platform/CCApplication.h"
#include "base/View.h"
#include "base/CCDirector.h"
#include "platform/CCFileUtils.h"
#include "platform/CCGLView.h"
#include "base/CCScriptSupport.h"

#include "bx/timer.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#include <shellapi.h>
#include <WinVer.h>
/**
@brief    This function change the PVRFrame show/hide setting in register.
@param  bEnable If true show the PVRFrame window, otherwise hide.
*/
static void PVRFrameEnableControlWindow(bool bEnable);
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

NS_CC_BEGIN

// sharedApplication pointer

Application::Application()
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
: _instance(nullptr)
, _accelTable(nullptr)
, glView_(nullptr)
#else
: glView_(nullptr)
#endif
, frame_(0)
, _maxFPS(60)
, _minFPS(30)
, _fpsLimited(true)
, _deltaTime(0)
, _cpuTime(0)
, _totalTime(0)
, _frequency(double(bx::getHPFrequency()))
, _appDelegate(nullptr)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    _instance  = GetModuleHandle(nullptr);
    _animationInterval.QuadPart = 0;
#endif
    _lastTime = bx::getHPFrequency() / _frequency;
}

Application::~Application()
{
}

int Application::mainLogic(bx::Thread* thread, void* userData)
{
    Application* app = reinterpret_cast<Application*>(userData);

    auto director = &SharedDirector;
    //app->setMaxFPS(60.0f);

    if (!SharedBGFX.init())
    {
        CCLOG("bgfx failed to initialize.");
        return 1;
    }

    if (!app->_appDelegate->applicationDidFinishLaunching())
    {
        return 1;
    }

    // Main message loop:

    auto glview = director->getOpenGLView();
    if (!glview)
    {
        director->setOpenGLView(app->glView_);
    }

    // Retain glview to avoid glview being released in the while loop
    glview->retain();

    // pass one frame
    SharedView.sandwichName("Main"_slice, []() {});
    app->frame_ = bgfx::frame();
    app->updateDeltaTime();
    app->makeTimeNow();

    while (!glview->windowShouldClose())
    {
        for (Own<QEvent> event = app->logicEvent_.poll();
            event != nullptr; event = app->logicEvent_.poll())
        {
            switch (Switch::hash(event->getName()))
            {
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
            case "Mouse"_hash:
            {
                int button, action, modify;
                event->get(button, action, modify);
                glview->mouseEvent(button, action, modify);
            }
            break;
            case "MouseMove"_hash:
            {
                intptr_t windowPtr;
                double x, y;
                event->get(windowPtr, x, y);
                glview->mouseMoveEvent(reinterpret_cast<void*>(windowPtr), x, y);
            }
            break;
            case "MouseScroll"_hash:
            {
                intptr_t windowPtr;
                double x, y;
                event->get(windowPtr, x, y);
                glview->mouseScrollEvent(reinterpret_cast<void*>(windowPtr), x, y);
            }
            break;
            case "Key"_hash:
            {
                intptr_t windowPtr;
                int key, scancode, action, mods;
                event->get(windowPtr, key, scancode, action, mods);
                glview->keyEvent(reinterpret_cast<void*>(windowPtr), key, scancode, action, mods);
            }
            break;
            case "Char"_hash:
            {
                intptr_t windowPtr;
                uint32_t character;
                event->get(windowPtr, character);
                glview->charEvent(reinterpret_cast<void*>(windowPtr), character);
            }
            break;
            case "WindowPos"_hash:
            {
                intptr_t windowPtr;
                int x, y;
                event->get(windowPtr, x, y);
                glview->posEvent(reinterpret_cast<void*>(windowPtr), x, y);
            }
            break;
            case "FramebufferSize"_hash:
            {
                intptr_t windowPtr;
                int width, height;
                event->get(windowPtr, width, height);
                glview->framebufferSizeEvent(reinterpret_cast<void*>(windowPtr), width, height);
            }
            break;
            case "WindowSize"_hash:
            {
                intptr_t windowPtr;
                int width, height;
                event->get(windowPtr, width, height);
                glview->sizeEvent(reinterpret_cast<void*>(windowPtr), width, height);
            }
            break;
            case "Iconify"_hash:
            {
                intptr_t windowPtr;
                int iconified;
                event->get(windowPtr, iconified);
                glview->iconifyEvent(reinterpret_cast<void*>(windowPtr), iconified);
            }
            break;
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
            case "TouchesBegin"_hash:
            {
                int num;
                TouchEventBEC ev;
                event->get(num, ev);
                glview->handleTouchesBegin(num, ev.ids, ev.xs, ev.ys);
            }
            break;
            case "TouchesMove"_hash:
            {
                int num;
                TouchEventMove ev;
                event->get(num, ev);
                glview->handleTouchesMove(num, ev.ids, ev.xs, ev.ys, ev.fs, ev.ms);
            }
            break;
            case "TouchesEnd"_hash:
            {
                int num;
                TouchEventBEC ev;
                event->get(num, ev);
                glview->handleTouchesEnd(num, ev.ids, ev.xs, ev.ys);
            }
            break;
            case "TouchesCancel"_hash:
            {
                int num;
                TouchEventBEC ev;
                event->get(num, ev);
                glview->handleTouchesCancel(num, ev.ids, ev.xs, ev.ys);
            }
            break;
#endif
            case "Invoke"_hash:
            {
                std::function<void()> func;
                event->get(func);
                func();
            }
            break;
            }
        }

        director->mainLoop();

        app->_cpuTime = app->getElapsedTime();
        // advance to next frame. rendering thread will be kicked to
        // process submitted rendering primitives.
        app->frame_ = bgfx::frame();

        // limit for max FPS
        if (app->_fpsLimited)
        {
            app->updateDeltaTime();
            while (app->getDeltaTime() < 1.0 / app->_maxFPS)
            {
                int32_t waitMS = (1.0 / app->_maxFPS - app->getDeltaTime()) * 1000;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
                Sleep(waitMS);
#endif
                app->updateDeltaTime();
            }
        }
        else
        {
            app->updateDeltaTime();
        }
        app->makeTimeNow();
    }

    // Director should still do a cleanup if the window was closed manually.
    if (glview->isOpenGLReady())
    {
        director->end();
        director->mainLoop();
        ScriptEngineManager::destroyInstance();
    }

    Life::destroy("BGFXCocos"_slice);
    return 0;
}
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
int Application::run()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    PVRFrameEnableControlWindow(false);
#endif

    // Initialize instance and cocos2d.
#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
    cocos2d::GLView* glView = SharedDirector.getOpenGLView();
    if (!glView)
    {
        if (!(glView_ = _appDelegate->initView()))
        {
            return 1;
        }
        SharedDirector.setOpenGLView(glView_);
    }
    else
    {
        glView_ = glView;
    }
#else
    if (!_appDelegate->initView())
    {
        return 1;
    }
#endif
    // call this function here to disable default render threads creation of bgfx
    Application::renderFrame();

    // start running logic thread
    _logicThread.init(Application::mainLogic, this);


    while (!glView_->windowShouldClose())
    {
        glView_->pollEvents();

        for (Own<QEvent> event = renderEvent_.poll();
            event != nullptr;
            event = renderEvent_.poll())
        {
            switch (Switch::hash(event->getName()))
            {
            case "Invoke"_hash:
            {
                std::function<void()> func;
                event->get(func);
                func();
            }
            break;
            }
        }

        Application::renderFrame();
    }

    // wait for render process to stop
    while (bgfx::RenderFrame::NoContext != Application::renderFrame());
    _logicThread.shutdown();

    glView_->release();

    return _logicThread.getExitCode();
}
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
void Application::iOSEventLoop()
{
    for (Own<QEvent> event = renderEvent_.poll();
         event != nullptr;
         event = renderEvent_.poll())
    {
        switch (Switch::hash(event->getName()))
        {
            case "Invoke"_hash:
            {
                std::function<void()> func;
                event->get(func);
                func();
            }
                break;
        }
    }
    
    Application::renderFrame();
}
#endif


void Application::setStartupScriptFilename(const std::string& startupScriptFile)
{
    _startupScriptFilename = startupScriptFile;
    std::replace(_startupScriptFilename.begin(), _startupScriptFilename.end(), '\\', '/');
}

void Application::invokeInRenderer(const std::function<void()>& func)
{
    renderEvent_.post("Invoke"_slice, func);
}

void Application::invokeInLogic(const std::function<void()>& func)
{
    logicEvent_.post("Invoke"_slice, func);
}

void Application::setAppDelegate(ApplicationProtocol* app)
{
    _appDelegate = app;
}

#if BX_PLATFORM_WINDOWS

void Application::setAnimationInterval(float interval)
{
    LARGE_INTEGER nFreq;
    QueryPerformanceFrequency(&nFreq);
    _animationInterval.QuadPart = (LONGLONG)(interval * nFreq.QuadPart);
}

LanguageType Application::getCurrentLanguage()
{
    LanguageType ret = LanguageType::ENGLISH;

    LCID localeID = GetUserDefaultLCID();
    unsigned short primaryLanguageID = localeID & 0xFF;

    switch (primaryLanguageID)
    {
    case LANG_CHINESE:
        ret = LanguageType::CHINESE;
        break;
    case LANG_ENGLISH:
        ret = LanguageType::ENGLISH;
        break;
    case LANG_FRENCH:
        ret = LanguageType::FRENCH;
        break;
    case LANG_ITALIAN:
        ret = LanguageType::ITALIAN;
        break;
    case LANG_GERMAN:
        ret = LanguageType::GERMAN;
        break;
    case LANG_SPANISH:
        ret = LanguageType::SPANISH;
        break;
    case LANG_DUTCH:
        ret = LanguageType::DUTCH;
        break;
    case LANG_RUSSIAN:
        ret = LanguageType::RUSSIAN;
        break;
    case LANG_KOREAN:
        ret = LanguageType::KOREAN;
        break;
    case LANG_JAPANESE:
        ret = LanguageType::JAPANESE;
        break;
    case LANG_HUNGARIAN:
        ret = LanguageType::HUNGARIAN;
        break;
    case LANG_PORTUGUESE:
        ret = LanguageType::PORTUGUESE;
        break;
    case LANG_ARABIC:
        ret = LanguageType::ARABIC;
        break;
    case LANG_NORWEGIAN:
        ret = LanguageType::NORWEGIAN;
        break;
    case LANG_POLISH:
        ret = LanguageType::POLISH;
        break;
    case LANG_TURKISH:
        ret = LanguageType::TURKISH;
        break;
    case LANG_UKRAINIAN:
        ret = LanguageType::UKRAINIAN;
        break;
    case LANG_ROMANIAN:
        ret = LanguageType::ROMANIAN;
        break;
    case LANG_BULGARIAN:
        ret = LanguageType::BULGARIAN;
        break;
    }

    return ret;
}

const char * Application::getCurrentLanguageCode()
{
    LANGID lid = GetUserDefaultUILanguage();
    const LCID locale_id = MAKELCID(lid, SORT_DEFAULT);
    static char code[3] = { 0 };
    GetLocaleInfoA(locale_id, LOCALE_SISO639LANGNAME, code, sizeof(code));
    code[2] = '\0';
    return code;
}

Application::Platform Application::getTargetPlatform()
{
    return Platform::OS_WINDOWS;
}

std::string Application::getVersion()
{
    char verString[256] = { 0 };
    TCHAR szVersionFile[MAX_PATH];
    GetModuleFileName(NULL, szVersionFile, MAX_PATH);
    DWORD  verHandle = NULL;
    UINT   size = 0;
    LPBYTE lpBuffer = NULL;
    DWORD  verSize = GetFileVersionInfoSize(szVersionFile, &verHandle);

    if (verSize != NULL)
    {
        LPSTR verData = new char[verSize];

        if (GetFileVersionInfo(szVersionFile, verHandle, verSize, verData))
        {
            if (VerQueryValue(verData, L"\\", (VOID FAR* FAR*)&lpBuffer, &size))
            {
                if (size)
                {
                    VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO *)lpBuffer;
                    if (verInfo->dwSignature == 0xfeef04bd)
                    {

                        // Doesn't matter if you are on 32 bit or 64 bit,
                        // DWORD is always 32 bits, so first two revision numbers
                        // come from dwFileVersionMS, last two come from dwFileVersionLS
                        sprintf(verString, "%d.%d.%d.%d", (verInfo->dwFileVersionMS >> 16) & 0xffff,
                            (verInfo->dwFileVersionMS >> 0) & 0xffff,
                            (verInfo->dwFileVersionLS >> 16) & 0xffff,
                            (verInfo->dwFileVersionLS >> 0) & 0xffff
                        );
                    }
                }
            }
        }
        delete[] verData;
    }
    return verString;
}

bool Application::openURL(const std::string &url)
{
    WCHAR *temp = new WCHAR[url.size() + 1];
    int wchars_num = MultiByteToWideChar(CP_UTF8, 0, url.c_str(), url.size() + 1, temp, url.size() + 1);
    HINSTANCE r = ShellExecuteW(NULL, L"open", temp, NULL, NULL, SW_SHOWNORMAL);
    delete[] temp;
    return (size_t)r > 32;
}


const char* Application::getMacAddress()
{
	return m_macAddress.c_str();
}

const char* Application::getAdvertisingIdentifier()
{
	return m_advertisingIdentifier.c_str();
}

const char* Application::getOpenUDID()
{
	return m_openUDID.c_str();
}

const char* Application::getIMEI()
{
	return m_imei.c_str();
}

const char* Application::getUDID()
{
	return m_udid.c_str();
}

int Application::getAvailableMemory()
{
    return 400;
}
#endif

double Application::getLastTime() const
{
    return _lastTime;
}

double Application::getDeltaTime() const
{
    return _deltaTime;
}

double Application::getElapsedTime() const
{
    double currentTime = bx::getHPCounter() / _frequency;
    return std::max(currentTime - _lastTime, 0.0);
}

double Application::getCurrentTime() const
{
    return bx::getHPCounter() / _frequency;
}

double Application::getCPUTime() const
{
    return _cpuTime;
}

double Application::getTotalTime() const
{
    return _totalTime;
}

void Application::setMaxFPS(uint32_t var)
{
    _maxFPS = var;
    SharedDirector.setAnimationInterval(1.0f / var);
}

uint32_t Application::getMaxFPS() const
{
    return _maxFPS;
}

void Application::setMinFPS(uint32_t var)
{
    _minFPS = var;
}

uint32_t Application::getMinFPS() const
{
    return _minFPS;
}

void Application::updateDeltaTime()
{
    double currentTime = bx::getHPCounter() / _frequency;
    _deltaTime = currentTime - _lastTime;
    if (_deltaTime < 0)
    {
        _deltaTime = 0;
        _lastTime = currentTime;
    }
}

void Application::makeTimeNow()
{
    _totalTime += _deltaTime;
    _lastTime = bx::getHPCounter() / _frequency;
}
#if BX_PLATFORM_WINDOWS || BX_PLATFORM_ANDROID
bgfx::RenderFrame::Enum Application::renderFrame()
{
    return bgfx::renderFrame();
}
#endif // BX_PLATFORM_WINDOWS || BX_PLATFORM_ANDROID
///////////////////////////////////////////////////////////////////////////


bool BGFXCocos::init()
{
    bgfx::Init init;
    init.type = bgfx::RendererType::OpenGL;
    init.debug = true;
    init.vendorId = 0;
    init.deviceId = 0;
    init.callback = nullptr;
    init.allocator = nullptr;
    return bgfx::init(init);
    return bgfx::init();
}

BGFXCocos::~BGFXCocos()
{
    bgfx::shutdown();
}

NS_CC_END

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
//////////////////////////////////////////////////////////////////////////
// Local function
//////////////////////////////////////////////////////////////////////////
static void PVRFrameEnableControlWindow(bool bEnable)
{
    HKEY hKey = 0;

    // Open PVRFrame control key, if not exist create it.
    if(ERROR_SUCCESS != RegCreateKeyExW(HKEY_CURRENT_USER,
        L"Software\\Imagination Technologies\\PVRVFRame\\STARTUP\\",
        0,
        0,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,
        0,
        &hKey,
        nullptr))
    {
        return;
    }

    const WCHAR* wszValue = L"hide_gui";
    const WCHAR* wszNewData = (bEnable) ? L"NO" : L"YES";
    WCHAR wszOldData[256] = {0};
    DWORD   dwSize = sizeof(wszOldData);
    LSTATUS status = RegQueryValueExW(hKey, wszValue, 0, nullptr, (LPBYTE)wszOldData, &dwSize);
    if (ERROR_FILE_NOT_FOUND == status              // the key not exist
        || (ERROR_SUCCESS == status                 // or the hide_gui value is exist
        && 0 != wcscmp(wszNewData, wszOldData)))    // but new data and old data not equal
    {
        dwSize = sizeof(WCHAR) * (wcslen(wszNewData) + 1);
        RegSetValueEx(hKey, wszValue, 0, REG_SZ, (const BYTE *)wszNewData, dwSize);
    }

    RegCloseKey(hKey);
}
#endif
