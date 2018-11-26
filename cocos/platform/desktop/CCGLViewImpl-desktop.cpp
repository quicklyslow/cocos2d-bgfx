/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.

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
#include "platform/desktop/CCGLViewImpl-desktop.h"

#include "platform/CCApplication.h"
#include "base/CCDirector.h"
#include "base/CCTouch.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventKeyboard.h"
#include "base/CCEventMouse.h"
#include "base/CCIMEDispatcher.h"
#include "base/ccUtils.h"
#include "base/ccUTF8.h"

NS_CC_BEGIN

// GLFWEventHandler

class GLFWEventHandler
{
public:
    static void onGLFWError(int errorID, const char* errorDesc)
    {
        if (_view)
            _view->onGLFWError(errorID, errorDesc);
    }

    static void onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify)
    {
        SharedApplication.postEventToLogic("Mouse"_slice, button, action, modify);
    }

    static void onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
    {
        SharedApplication.postEventToLogic("MouseMove"_slice, reinterpret_cast<intptr_t>(window), x, y);
    }

    static void onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
    {
        SharedApplication.postEventToLogic("MouseScroll"_slice, reinterpret_cast<intptr_t>(window), x, y);
    }

    static void onGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        SharedApplication.postEventToLogic("Key"_slice, reinterpret_cast<intptr_t>(window), key, scancode, action, mods);
    }

    static void onGLFWCharCallback(GLFWwindow* window, uint32_t character)
    {
        SharedApplication.postEventToLogic("Char"_slice, reinterpret_cast<intptr_t>(window), character);
    }

    static void onGLFWWindowPosCallback(GLFWwindow* window, int x, int y)
    {
        SharedApplication.postEventToLogic("WindowPos"_slice, reinterpret_cast<intptr_t>(window), x, y);
    }

    static void onGLFWframebuffersize(GLFWwindow* window, int w, int h)
    {
        SharedApplication.postEventToLogic("FramebufferSize"_slice, reinterpret_cast<intptr_t>(window), w, h);
    }

    static void onGLFWWindowSizeFunCallback(GLFWwindow *window, int width, int height)
    {
        SharedApplication.postEventToLogic("WindowSize"_slice, reinterpret_cast<intptr_t>(window), width, height);
    }

    static void setGLViewImpl(GLViewImpl* view)
    {
        _view = view;
    }

    static void onGLFWWindowIconifyCallback(GLFWwindow* window, int iconified)
    {
        SharedApplication.postEventToLogic("Iconify"_slice, reinterpret_cast<intptr_t>(window), iconified);
    }

private:
    static GLViewImpl* _view;
};

GLViewImpl* GLFWEventHandler::_view = nullptr;

////////////////////////////////////////////////////

struct keyCodeItem
{
    int glfwKeyCode;
    KeyCode keyCode;
};

static std::unordered_map<int, KeyCode> g_keyCodeMap;

static keyCodeItem g_keyCodeStructArray[] = {
    /* The unknown key */
    { GLFW_KEY_UNKNOWN         , KeyCode::KEY_NONE          },

    /* Printable keys */
    { GLFW_KEY_SPACE           , KeyCode::KEY_SPACE         },
    { GLFW_KEY_APOSTROPHE      , KeyCode::KEY_APOSTROPHE    },
    { GLFW_KEY_COMMA           , KeyCode::KEY_COMMA         },
    { GLFW_KEY_MINUS           , KeyCode::KEY_MINUS         },
    { GLFW_KEY_PERIOD          , KeyCode::KEY_PERIOD        },
    { GLFW_KEY_SLASH           , KeyCode::KEY_SLASH         },
    { GLFW_KEY_0               , KeyCode::KEY_0             },
    { GLFW_KEY_1               , KeyCode::KEY_1             },
    { GLFW_KEY_2               , KeyCode::KEY_2             },
    { GLFW_KEY_3               , KeyCode::KEY_3             },
    { GLFW_KEY_4               , KeyCode::KEY_4             },
    { GLFW_KEY_5               , KeyCode::KEY_5             },
    { GLFW_KEY_6               , KeyCode::KEY_6             },
    { GLFW_KEY_7               , KeyCode::KEY_7             },
    { GLFW_KEY_8               , KeyCode::KEY_8             },
    { GLFW_KEY_9               , KeyCode::KEY_9             },
    { GLFW_KEY_SEMICOLON       , KeyCode::KEY_SEMICOLON     },
    { GLFW_KEY_EQUAL           , KeyCode::KEY_EQUAL         },
    { GLFW_KEY_A               , KeyCode::KEY_A             },
    { GLFW_KEY_B               , KeyCode::KEY_B             },
    { GLFW_KEY_C               , KeyCode::KEY_C             },
    { GLFW_KEY_D               , KeyCode::KEY_D             },
    { GLFW_KEY_E               , KeyCode::KEY_E             },
    { GLFW_KEY_F               , KeyCode::KEY_F             },
    { GLFW_KEY_G               , KeyCode::KEY_G             },
    { GLFW_KEY_H               , KeyCode::KEY_H             },
    { GLFW_KEY_I               , KeyCode::KEY_I             },
    { GLFW_KEY_J               , KeyCode::KEY_J             },
    { GLFW_KEY_K               , KeyCode::KEY_K             },
    { GLFW_KEY_L               , KeyCode::KEY_L             },
    { GLFW_KEY_M               , KeyCode::KEY_M             },
    { GLFW_KEY_N               , KeyCode::KEY_N             },
    { GLFW_KEY_O               , KeyCode::KEY_O             },
    { GLFW_KEY_P               , KeyCode::KEY_P             },
    { GLFW_KEY_Q               , KeyCode::KEY_Q             },
    { GLFW_KEY_R               , KeyCode::KEY_R             },
    { GLFW_KEY_S               , KeyCode::KEY_S             },
    { GLFW_KEY_T               , KeyCode::KEY_T             },
    { GLFW_KEY_U               , KeyCode::KEY_U             },
    { GLFW_KEY_V               , KeyCode::KEY_V             },
    { GLFW_KEY_W               , KeyCode::KEY_W             },
    { GLFW_KEY_X               , KeyCode::KEY_X             },
    { GLFW_KEY_Y               , KeyCode::KEY_Y             },
    { GLFW_KEY_Z               , KeyCode::KEY_Z             },
    { GLFW_KEY_LEFT_BRACKET    , KeyCode::KEY_LEFT_BRACKET  },
    { GLFW_KEY_BACKSLASH       , KeyCode::KEY_BACK_SLASH    },
    { GLFW_KEY_RIGHT_BRACKET   , KeyCode::KEY_RIGHT_BRACKET },
    { GLFW_KEY_GRAVE_ACCENT    , KeyCode::KEY_GRAVE         },
    { GLFW_KEY_WORLD_1         , KeyCode::KEY_GRAVE         },
    { GLFW_KEY_WORLD_2         , KeyCode::KEY_NONE          },

    /* Function keys */
    { GLFW_KEY_ESCAPE          , KeyCode::KEY_ESCAPE        },
    { GLFW_KEY_ENTER           , KeyCode::KEY_ENTER      },
    { GLFW_KEY_TAB             , KeyCode::KEY_TAB           },
    { GLFW_KEY_BACKSPACE       , KeyCode::KEY_BACKSPACE     },
    { GLFW_KEY_INSERT          , KeyCode::KEY_INSERT        },
    { GLFW_KEY_DELETE          , KeyCode::KEY_DELETE        },
    { GLFW_KEY_RIGHT           , KeyCode::KEY_RIGHT_ARROW   },
    { GLFW_KEY_LEFT            , KeyCode::KEY_LEFT_ARROW    },
    { GLFW_KEY_DOWN            , KeyCode::KEY_DOWN_ARROW    },
    { GLFW_KEY_UP              , KeyCode::KEY_UP_ARROW      },
    { GLFW_KEY_PAGE_UP         , KeyCode::KEY_PG_UP      },
    { GLFW_KEY_PAGE_DOWN       , KeyCode::KEY_PG_DOWN    },
    { GLFW_KEY_HOME            , KeyCode::KEY_HOME       },
    { GLFW_KEY_END             , KeyCode::KEY_END           },
    { GLFW_KEY_CAPS_LOCK       , KeyCode::KEY_CAPS_LOCK     },
    { GLFW_KEY_SCROLL_LOCK     , KeyCode::KEY_SCROLL_LOCK   },
    { GLFW_KEY_NUM_LOCK        , KeyCode::KEY_NUM_LOCK      },
    { GLFW_KEY_PRINT_SCREEN    , KeyCode::KEY_PRINT         },
    { GLFW_KEY_PAUSE           , KeyCode::KEY_PAUSE         },
    { GLFW_KEY_F1              , KeyCode::KEY_F1            },
    { GLFW_KEY_F2              , KeyCode::KEY_F2            },
    { GLFW_KEY_F3              , KeyCode::KEY_F3            },
    { GLFW_KEY_F4              , KeyCode::KEY_F4            },
    { GLFW_KEY_F5              , KeyCode::KEY_F5            },
    { GLFW_KEY_F6              , KeyCode::KEY_F6            },
    { GLFW_KEY_F7              , KeyCode::KEY_F7            },
    { GLFW_KEY_F8              , KeyCode::KEY_F8            },
    { GLFW_KEY_F9              , KeyCode::KEY_F9            },
    { GLFW_KEY_F10             , KeyCode::KEY_F10           },
    { GLFW_KEY_F11             , KeyCode::KEY_F11           },
    { GLFW_KEY_F12             , KeyCode::KEY_F12           },
    { GLFW_KEY_F13             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F14             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F15             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F16             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F17             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F18             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F19             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F20             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F21             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F22             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F23             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F24             , KeyCode::KEY_NONE          },
    { GLFW_KEY_F25             , KeyCode::KEY_NONE          },
    { GLFW_KEY_KP_0            , KeyCode::KEY_0             },
    { GLFW_KEY_KP_1            , KeyCode::KEY_1             },
    { GLFW_KEY_KP_2            , KeyCode::KEY_2             },
    { GLFW_KEY_KP_3            , KeyCode::KEY_3             },
    { GLFW_KEY_KP_4            , KeyCode::KEY_4             },
    { GLFW_KEY_KP_5            , KeyCode::KEY_5             },
    { GLFW_KEY_KP_6            , KeyCode::KEY_6             },
    { GLFW_KEY_KP_7            , KeyCode::KEY_7             },
    { GLFW_KEY_KP_8            , KeyCode::KEY_8             },
    { GLFW_KEY_KP_9            , KeyCode::KEY_9             },
    { GLFW_KEY_KP_DECIMAL      , KeyCode::KEY_PERIOD        },
    { GLFW_KEY_KP_DIVIDE       , KeyCode::KEY_KP_DIVIDE     },
    { GLFW_KEY_KP_MULTIPLY     , KeyCode::KEY_KP_MULTIPLY   },
    { GLFW_KEY_KP_SUBTRACT     , KeyCode::KEY_KP_MINUS      },
    { GLFW_KEY_KP_ADD          , KeyCode::KEY_KP_PLUS       },
    { GLFW_KEY_KP_ENTER        , KeyCode::KEY_KP_ENTER      },
    { GLFW_KEY_KP_EQUAL        , KeyCode::KEY_EQUAL         },
    { GLFW_KEY_LEFT_SHIFT      , KeyCode::KEY_LEFT_SHIFT         },
    { GLFW_KEY_LEFT_CONTROL    , KeyCode::KEY_LEFT_CTRL          },
    { GLFW_KEY_LEFT_ALT        , KeyCode::KEY_LEFT_ALT           },
    { GLFW_KEY_LEFT_SUPER      , KeyCode::KEY_HYPER         },
    { GLFW_KEY_RIGHT_SHIFT     , KeyCode::KEY_RIGHT_SHIFT         },
    { GLFW_KEY_RIGHT_CONTROL   , KeyCode::KEY_RIGHT_CTRL          },
    { GLFW_KEY_RIGHT_ALT       , KeyCode::KEY_RIGHT_ALT           },
    { GLFW_KEY_RIGHT_SUPER     , KeyCode::KEY_HYPER         },
    { GLFW_KEY_MENU            , KeyCode::KEY_MENU          },
    { GLFW_KEY_LAST            , KeyCode::KEY_NONE          }
};

static EventMouse* __mouseDownEvent = nullptr;
static EventMouse* __mouseMoveEvent = nullptr;
static EventMouse* __mouseUpEvent = nullptr;
static EventMouse* __mouseScrollEvent = nullptr;

//////////////////////////////////////////////////////////////////////////
// implement GLViewImpl
//////////////////////////////////////////////////////////////////////////


GLViewImpl::GLViewImpl(bool initglfw)
: _captured(false)
, _supportTouch(false)
, _isInRetinaMonitor(false)
, _isRetinaEnabled(false)
, _retinaFactor(1)
, _frameZoomFactor(1.0f)
, _mainWindow(nullptr)
, _monitor(nullptr)
, _mouseX(0.0f)
, _mouseY(0.0f)
{
    _viewName = "cocos2dx";
    g_keyCodeMap.clear();
    for (auto& item : g_keyCodeStructArray)
    {
        g_keyCodeMap[item.glfwKeyCode] = item.keyCode;
    }
    __mouseDownEvent = new (std::nothrow) EventMouse(EventMouse::MouseEventType::MOUSE_DOWN);
    __mouseMoveEvent = new (std::nothrow) EventMouse(EventMouse::MouseEventType::MOUSE_MOVE);
    __mouseUpEvent = new (std::nothrow) EventMouse(EventMouse::MouseEventType::MOUSE_UP);
    __mouseScrollEvent = new (std::nothrow) EventMouse(EventMouse::MouseEventType::MOUSE_SCROLL);

    GLFWEventHandler::setGLViewImpl(this);
    if (initglfw)
    {
        glfwSetErrorCallback(GLFWEventHandler::onGLFWError);
        glfwInit();
    }
}

GLViewImpl::~GLViewImpl()
{
    CCLOGINFO("deallocing GLViewImpl: %p", this);
    GLFWEventHandler::setGLViewImpl(nullptr);
    glfwTerminate();

    CC_SAFE_RELEASE_NULL(__mouseDownEvent);
    CC_SAFE_RELEASE_NULL(__mouseMoveEvent);
    CC_SAFE_RELEASE_NULL(__mouseUpEvent);
    CC_SAFE_RELEASE_NULL(__mouseScrollEvent);
}

GLViewImpl* GLViewImpl::create(const std::string& viewName)
{
    return GLViewImpl::create(viewName, false);
}

GLViewImpl* GLViewImpl::create(const std::string& viewName, bool resizable)
{
    auto ret = new (std::nothrow) GLViewImpl;
    if(ret && ret->initWithRect(viewName, Rect(0, 0, 960, 640), 1.0f, resizable)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::createWithRect(const std::string& viewName, const Rect& rect, float frameZoomFactor, bool resizable/* = false*/)
{
    auto ret = new (std::nothrow) GLViewImpl;
    if(ret && ret->initWithRect(viewName, rect, frameZoomFactor, resizable)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::createWithFullScreen(const std::string& viewName)
{
    auto ret = new (std::nothrow) GLViewImpl();
    if(ret && ret->initWithFullScreen(viewName)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl* GLViewImpl::createWithFullScreen(const std::string& viewName, const GLFWvidmode &videoMode, GLFWmonitor *monitor)
{
    auto ret = new (std::nothrow) GLViewImpl();
    if(ret && ret->initWithFullscreen(viewName, videoMode, monitor)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GLViewImpl::initWithRect(const std::string& viewName, Rect rect, float frameZoomFactor, bool resizable)
{
    setViewName(viewName);

    _frameZoomFactor = frameZoomFactor;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    int neededWidth = rect.size.width * _frameZoomFactor;
    int neededHeight = rect.size.height * _frameZoomFactor;

    _mainWindow = glfwCreateWindow(neededWidth, neededHeight, _viewName.c_str(), _monitor, nullptr, nullptr);

    if (_mainWindow == nullptr)
    {
        std::string message = "Can't create window";
        if (!_glfwError.empty())
        {
            message.append("\nMore info: \n");
            message.append(_glfwError);
        }

        MessageBox(message.c_str(), "Error launch application");
        return false;
    }

    /*
    *  Note that the created window and context may differ from what you requested,
    *  as not all parameters and hints are
    *  [hard constraints](@ref window_hints_hard).  This includes the size of the
    *  window, especially for full screen windows.  To retrieve the actual
    *  attributes of the created window and context, use queries like @ref
    *  glfwGetWindowAttrib and @ref glfwGetWindowSize.
    *
    *  see declaration glfwCreateWindow
    */
    int realW = 0, realH = 0;
    glfwGetWindowSize(_mainWindow, &realW, &realH);
    if (realW != neededWidth)
    {
        rect.size.width = realW / _frameZoomFactor;
    }
    if (realH != neededHeight)
    {
        rect.size.height = realH / _frameZoomFactor;
    }

    glfwSetMouseButtonCallback(_mainWindow, GLFWEventHandler::onGLFWMouseCallBack);
    glfwSetCursorPosCallback(_mainWindow, GLFWEventHandler::onGLFWMouseMoveCallBack);
    glfwSetScrollCallback(_mainWindow, GLFWEventHandler::onGLFWMouseScrollCallback);
    glfwSetCharCallback(_mainWindow, GLFWEventHandler::onGLFWCharCallback);
    glfwSetKeyCallback(_mainWindow, GLFWEventHandler::onGLFWKeyCallback);
    glfwSetWindowPosCallback(_mainWindow, GLFWEventHandler::onGLFWWindowPosCallback);
    glfwSetFramebufferSizeCallback(_mainWindow, GLFWEventHandler::onGLFWframebuffersize);
    glfwSetWindowSizeCallback(_mainWindow, GLFWEventHandler::onGLFWWindowSizeFunCallback);
    glfwSetWindowIconifyCallback(_mainWindow, GLFWEventHandler::onGLFWWindowIconifyCallback);

    setFrameSize(rect.size.width, rect.size.height);


    bgfx::PlatformData platformData;
    memset(&platformData, 0, sizeof(platformData));
    platformData.nwh = getWin32Window();
    bgfx::setPlatformData(platformData);



//    // GLFW v3.2 no longer emits "onGLFWWindowSizeFunCallback" at creation time. Force default viewport:
//    setViewPortInPoints(0, 0, neededWidth, neededHeight);
//
    return true;
}

bool GLViewImpl::initWithFullScreen(const std::string& viewName)
{
    //Create fullscreen window on primary monitor at its current video mode.
    _monitor = glfwGetPrimaryMonitor();
    if (nullptr == _monitor)
        return false;

    const GLFWvidmode* videoMode = glfwGetVideoMode(_monitor);
    return initWithRect(viewName, Rect(0, 0, videoMode->width, videoMode->height), 1.0f, false);
}

bool GLViewImpl::initWithFullscreen(const std::string &viewname, const GLFWvidmode &videoMode, GLFWmonitor *monitor)
{
    //Create fullscreen on specified monitor at the specified video mode.
    _monitor = monitor;
    if (nullptr == _monitor)
        return false;

    //These are soft contraints. If the video mode is retrieved at runtime, the resulting window and context should match these exactly. If invalid attribs are passed (eg. from an outdated cache), window creation will NOT fail but the actual window/context may differ.
    glfwWindowHint(GLFW_REFRESH_RATE, videoMode.refreshRate);
    glfwWindowHint(GLFW_RED_BITS, videoMode.redBits);
    glfwWindowHint(GLFW_BLUE_BITS, videoMode.blueBits);
    glfwWindowHint(GLFW_GREEN_BITS, videoMode.greenBits);

    return initWithRect(viewname, Rect(0, 0, videoMode.width, videoMode.height), 1.0f, false);
}

bool GLViewImpl::isOpenGLReady()
{
    return nullptr != _mainWindow;
}

void GLViewImpl::end()
{
    if(_mainWindow)
    {
        glfwSetWindowShouldClose(_mainWindow,1);
        _mainWindow = nullptr;
    }
    // Release self. Otherwise, GLViewImpl could not be freed.
    release();
}

void GLViewImpl::swapBuffers()
{
    if(_mainWindow)
        glfwSwapBuffers(_mainWindow);
}

bool GLViewImpl::windowShouldClose()
{
    if(_mainWindow)
        return glfwWindowShouldClose(_mainWindow) ? true : false;
    else
        return true;
}

void GLViewImpl::pollEvents()
{
    glfwPollEvents();
}

void GLViewImpl::enableRetina(bool enabled)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    _isRetinaEnabled = enabled;
    if (_isRetinaEnabled)
    {
        _retinaFactor = 1;
    }
    else
    {
        _retinaFactor = 2;
    }
    updateFrameSize();
#endif
}


void GLViewImpl::setIMEKeyboardState(bool /*bOpen*/)
{

}

void GLViewImpl::setCursorVisible( bool isVisible )
{
    if( _mainWindow == NULL )
        return;

    if( isVisible )
        glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(_mainWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void GLViewImpl::setFrameZoomFactor(float zoomFactor)
{
    CCASSERT(zoomFactor > 0.0f, "zoomFactor must be larger than 0");

    if (std::abs(_frameZoomFactor - zoomFactor) < FLT_EPSILON)
    {
        return;
    }

    _frameZoomFactor = zoomFactor;
    updateFrameSize();
}

float GLViewImpl::getFrameZoomFactor() const
{
    return _frameZoomFactor;
}

void GLViewImpl::updateFrameSize()
{
    if (_screenSize.width > 0 && _screenSize.height > 0)
    {
        int w = 0, h = 0;
        glfwGetWindowSize(_mainWindow, &w, &h);

        int frameBufferW = 0, frameBufferH = 0;
        glfwGetFramebufferSize(_mainWindow, &frameBufferW, &frameBufferH);

        if (frameBufferW == 2 * w && frameBufferH == 2 * h)
        {
            if (_isRetinaEnabled)
            {
                _retinaFactor = 1;
            }
            else
            {
                _retinaFactor = 2;
            }
            SharedApplication.invokeInRenderer([&]()
            {
                glfwSetWindowSize(_mainWindow, _screenSize.width / 2 * _retinaFactor * _frameZoomFactor, _screenSize.height / 2 * _retinaFactor * _frameZoomFactor);
            });
            _isInRetinaMonitor = true;
        }
        else
        {
            if (_isInRetinaMonitor)
            {
                _retinaFactor = 1;
            }
            SharedApplication.invokeInRenderer([&]()
            {
                glfwSetWindowSize(_mainWindow, _screenSize.width * _retinaFactor * _frameZoomFactor, _screenSize.height *_retinaFactor * _frameZoomFactor);
            });

            _isInRetinaMonitor = false;
        }
    }
}

void GLViewImpl::setFrameSize(float width, float height)
{
    GLView::setFrameSize(width, height);
    updateFrameSize();
}

void GLViewImpl::setViewPortInPoints(float x , float y , float w , float h)
{
    glViewport((GLint)(x * _scaleX * _retinaFactor * _frameZoomFactor + _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor),
        (GLint)(y * _scaleY * _retinaFactor  * _frameZoomFactor + _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor),
        (GLint)(w * _scaleX * _retinaFactor * _frameZoomFactor),
        (GLint)(h * _scaleY * _retinaFactor * _frameZoomFactor));
}

void GLViewImpl::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor((GLint)(x * _scaleX * _retinaFactor * _frameZoomFactor + _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor),
               (GLint)(y * _scaleY * _retinaFactor  * _frameZoomFactor + _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor),
               (GLsizei)(w * _scaleX * _retinaFactor * _frameZoomFactor),
               (GLsizei)(h * _scaleY * _retinaFactor * _frameZoomFactor));
}

Rect GLViewImpl::getScissorRect() const
{
    GLfloat params[4];
    glGetFloatv(GL_SCISSOR_BOX, params);
    float x = (params[0] - _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor) / (_scaleX * _retinaFactor * _frameZoomFactor);
    float y = (params[1] - _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor) / (_scaleY * _retinaFactor  * _frameZoomFactor);
    float w = params[2] / (_scaleX * _retinaFactor * _frameZoomFactor);
    float h = params[3] / (_scaleY * _retinaFactor  * _frameZoomFactor);
    return Rect(x, y, w, h);
}

void GLViewImpl::onGLFWError(int errorID, const char* errorDesc)
{
    if (_mainWindow)
    {
        _glfwError = StringUtils::format("GLFWError #%d Happen, %s", errorID, errorDesc);
    }
    else
    {
        _glfwError.append(StringUtils::format("GLFWError #%d Happen, %s\n", errorID, errorDesc));
    }
    CCLOGERROR("%s", _glfwError.c_str());
}

void GLViewImpl::mouseEvent(int button, int action, int modify)
{
    onGLFWMouseCallBack(nullptr, button, action, modify);
}

void GLViewImpl::mouseMoveEvent(void* window, double x, double y)
{
    onGLFWMouseMoveCallBack(static_cast<GLFWwindow*>(window), x, y);
}

void GLViewImpl::mouseScrollEvent(void* window, double x, double y)
{
    onGLFWMouseScrollCallback(static_cast<GLFWwindow*>(window), x, y);
}

void GLViewImpl::keyEvent(void* window, int key, int scancode, int action, int mods)
{
    onGLFWKeyCallback(static_cast<GLFWwindow*>(window), key, scancode, action, mods);
}

void GLViewImpl::charEvent(void* window, uint32_t character)
{
    onGLFWCharCallback(static_cast<GLFWwindow*>(window), character);
}

void GLViewImpl::posEvent(void* window, int x, int y)
{
    onGLFWWindowPosCallback(static_cast<GLFWwindow*>(window), x, y);
}

void GLViewImpl::iconifyEvent(void* window, int iconified)
{
    onGLFWWindowIconifyCallback(static_cast<GLFWwindow*>(window), iconified);
}

void GLViewImpl::sizeEvent(void* window, int width, int height)
{
    onGLFWWindowSizeFunCallback(static_cast<GLFWwindow*>(window), width, height);
}

void GLViewImpl::framebufferSizeEvent(void* window, int width, int height)
{
    onGLFWframebuffersize(static_cast<GLFWwindow*>(window), width, height);
}

void GLViewImpl::onGLFWMouseCallBack(GLFWwindow* window, int button, int action, int modify)
{
    if(GLFW_MOUSE_BUTTON_LEFT == button)
    {
        if(GLFW_PRESS == action)
        {
            _captured = true;
            if (this->getViewPortRect().equals(Rect::ZERO) || this->getViewPortRect().containsPoint(Vec2(_mouseX,_mouseY)))
            {
                intptr_t id = 0;
                this->handleTouchesBegin(1, &id, &_mouseX, &_mouseY);
            }
        }
        else if(GLFW_RELEASE == action)
        {
            if (_captured)
            {
                _captured = false;
                intptr_t id = 0;
                this->handleTouchesEnd(1, &id, &_mouseX, &_mouseY);
            }
        }
    }

    //Because OpenGL and cocos2d-x uses different Y axis, we need to convert the coordinate here
    float cursorX = (_mouseX - _viewPortRect.origin.x) / _scaleX;
    float cursorY = (_viewPortRect.origin.y + _viewPortRect.size.height - _mouseY) / _scaleY;

    if(GLFW_PRESS == action)
    {
        EventMouse* event = __mouseDownEvent;
        event->setCursorPosition(cursorX, cursorY);
        event->setMouseButton(button);
        SharedDirector.getEventDispatcher()->dispatchEvent(event);
    }
    else if(GLFW_RELEASE == action)
    {
        EventMouse* event = __mouseUpEvent;
        event->setCursorPosition(cursorX, cursorY);
        event->setMouseButton(button);
        SharedDirector.getEventDispatcher()->dispatchEvent(event);
    }
}

void GLViewImpl::onGLFWMouseMoveCallBack(GLFWwindow* window, double x, double y)
{
    _mouseX = (float)x;
    _mouseY = (float)y;

    _mouseX /= this->getFrameZoomFactor();
    _mouseY /= this->getFrameZoomFactor();

    if (_isInRetinaMonitor)
    {
        if (_retinaFactor == 1)
        {
            _mouseX *= 2;
            _mouseY *= 2;
        }
    }

    if (_captured)
    {
        intptr_t id = 0;
        this->handleTouchesMove(1, &id, &_mouseX, &_mouseY);
    }

    //Because OpenGL and cocos2d-x uses different Y axis, we need to convert the coordinate here
    float cursorX = (_mouseX - _viewPortRect.origin.x) / _scaleX;
    float cursorY = (_viewPortRect.origin.y + _viewPortRect.size.height - _mouseY) / _scaleY;

    EventMouse* event = __mouseMoveEvent;
    // Set current button
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        event->setMouseButton(GLFW_MOUSE_BUTTON_LEFT);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        event->setMouseButton(GLFW_MOUSE_BUTTON_RIGHT);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        event->setMouseButton(GLFW_MOUSE_BUTTON_MIDDLE);
    }
    event->setCursorPosition(cursorX, cursorY);
    SharedDirector.getEventDispatcher()->dispatchEvent(event);
}

void GLViewImpl::onGLFWMouseScrollCallback(GLFWwindow* window, double x, double y)
{
    EventMouse* event = __mouseScrollEvent;
    //Because OpenGL and cocos2d-x uses different Y axis, we need to convert the coordinate here
    float cursorX = (_mouseX - _viewPortRect.origin.x) / _scaleX;
    float cursorY = (_viewPortRect.origin.y + _viewPortRect.size.height - _mouseY) / _scaleY;
    event->setScrollData((float)x, -(float)y);
    event->setCursorPosition(cursorX, cursorY);
    SharedDirector.getEventDispatcher()->dispatchEvent(event);
}

void GLViewImpl::onGLFWKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (GLFW_REPEAT != action)
    {
        EventKeyboard* event = new (std::nothrow)EventKeyboard(g_keyCodeMap[key], GLFW_PRESS == action);
        auto dispatcher = SharedDirector.getEventDispatcher();
        dispatcher->dispatchEvent(event);
        event->release();
    }

    if (GLFW_RELEASE != action)
    {
        switch (g_keyCodeMap[key])
        {
        case KeyCode::KEY_BACKSPACE:
            IMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
            break;
        case KeyCode::KEY_HOME:
        case KeyCode::KEY_KP_HOME:
        case KeyCode::KEY_DELETE:
        case KeyCode::KEY_KP_DELETE:
        case KeyCode::KEY_END:
        case KeyCode::KEY_LEFT_ARROW:
        case KeyCode::KEY_RIGHT_ARROW:
        case KeyCode::KEY_ESCAPE:
            IMEDispatcher::sharedDispatcher()->dispatchControlKey(g_keyCodeMap[key]);
            break;
        default:
            break;
        }
    }
}

void GLViewImpl::onGLFWCharCallback(GLFWwindow *window, unsigned int character)
{
    char16_t wcharString[2] = { (char16_t) character, 0 };
    std::string utf8String;

    StringUtils::UTF16ToUTF8( wcharString, utf8String );
    static std::set<std::string> controlUnicode = {
        "\xEF\x9C\x80", // up
        "\xEF\x9C\x81", // down
        "\xEF\x9C\x82", // left
        "\xEF\x9C\x83", // right
        "\xEF\x9C\xA8", // delete
        "\xEF\x9C\xA9", // home
        "\xEF\x9C\xAB", // end
        "\xEF\x9C\xAC", // pageup
        "\xEF\x9C\xAD", // pagedown
        "\xEF\x9C\xB9"  // clear
    };
    // Check for send control key
    if (controlUnicode.find(utf8String) == controlUnicode.end())
    {
        IMEDispatcher::sharedDispatcher()->dispatchInsertText( utf8String.c_str(), utf8String.size() );
    }
}

void GLViewImpl::onGLFWWindowPosCallback(GLFWwindow *windows, int x, int y)
{
    //SharedDirector.setViewport();
}

void GLViewImpl::onGLFWframebuffersize(GLFWwindow* window, int w, int h)
{
    float frameSizeW = _screenSize.width;
    float frameSizeH = _screenSize.height;
    float factorX = frameSizeW / w * _retinaFactor * _frameZoomFactor;
    float factorY = frameSizeH / h * _retinaFactor * _frameZoomFactor;

    if (std::abs(factorX - 0.5f) < FLT_EPSILON && std::abs(factorY - 0.5f) < FLT_EPSILON)
    {
        _isInRetinaMonitor = true;
        if (_isRetinaEnabled)
        {
            _retinaFactor = 1;
        }
        else
        {
            _retinaFactor = 2;
        }
        SharedApplication.invokeInRenderer([&, frameSizeW, frameSizeH]()
        {
            glfwSetWindowSize(window, static_cast<int>(frameSizeW * 0.5f * _retinaFactor * _frameZoomFactor), static_cast<int>(frameSizeH * 0.5f * _retinaFactor * _frameZoomFactor));
        });
    }
    else if (std::abs(factorX - 2.0f) < FLT_EPSILON && std::abs(factorY - 2.0f) < FLT_EPSILON)
    {
        _isInRetinaMonitor = false;
        _retinaFactor = 1;
        SharedApplication.invokeInRenderer([&, frameSizeW, frameSizeH]()
        {
            glfwSetWindowSize(window, static_cast<int>(frameSizeW * _retinaFactor * _frameZoomFactor), static_cast<int>(frameSizeH * _retinaFactor * _frameZoomFactor));
        });
    }
}

void GLViewImpl::onGLFWWindowSizeFunCallback(GLFWwindow *window, int width, int height)
{
    if (width && height && _resolutionPolicy != ResolutionPolicy::UNKNOWN)
    {
        Size baseDesignSize = _designResolutionSize;
        ResolutionPolicy baseResolutionPolicy = _resolutionPolicy;

        int frameWidth = width / _frameZoomFactor;
        int frameHeight = height / _frameZoomFactor;
        setFrameSize(frameWidth, frameHeight);
        setDesignResolutionSize(baseDesignSize.width, baseDesignSize.height, baseResolutionPolicy);
        //SharedDirector.setViewport();
    }
}

void GLViewImpl::onGLFWWindowIconifyCallback(GLFWwindow* window, int iconified)
{
    if (iconified == GL_TRUE)
    {
        SharedApplication.applicationDidEnterBackground();
    }
    else
    {
        SharedApplication.applicationWillEnterForeground();
    }
}

NS_CC_END // end of namespace cocos2d;
