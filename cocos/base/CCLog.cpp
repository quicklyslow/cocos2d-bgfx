#include "ccHeader.h"
#include "CCLog.h"
#include "base/CCDirector.h"

namespace cocos2d 
{

    //
// Free functions to log
//

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    void SendLogToWindow(const char *log)
    {
        static const int CCLOG_STRING_TAG = 1;
        // Send data as a message
        COPYDATASTRUCT myCDS;
        myCDS.dwData = CCLOG_STRING_TAG;
        myCDS.cbData = (DWORD)strlen(log) + 1;
        myCDS.lpData = (PVOID)log;
        if (SharedDirector.getOpenGLView())
        {
            HWND hwnd = SharedDirector.getOpenGLView()->getWin32Window();
            SendMessage(hwnd,
                WM_COPYDATA,
                (WPARAM)(HWND)hwnd,
                (LPARAM)(LPVOID)&myCDS);
        }
    }
#endif

    void _log(const char *format, va_list args)
    {
        int bufferSize = MAX_LOG_LENGTH;
        char* buf = nullptr;
        
        do
        {
            buf = new (std::nothrow) char[bufferSize];
            if (buf == nullptr)
                return; // not enough memory
            
            int ret = vsnprintf(buf, bufferSize - 3, format, args);
            if (ret < 0)
            {
                bufferSize *= 2;
                
                delete [] buf;
            }
            else
                break;
            
        } while (true);
        
        strcat(buf, "\n");
        
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        __android_log_print(ANDROID_LOG_DEBUG, "cocos2d-x debug info", "%s", buf);
        
#elif CC_TARGET_PLATFORM ==  CC_PLATFORM_WIN32
        
        int pos = 0;
        int len = strlen(buf);
        char tempBuf[MAX_LOG_LENGTH + 1] = { 0 };
        WCHAR wszBuf[MAX_LOG_LENGTH + 1] = { 0 };
        
        do
        {
            std::copy(buf + pos, buf + pos + MAX_LOG_LENGTH, tempBuf);
            
            tempBuf[MAX_LOG_LENGTH] = 0;
            
            MultiByteToWideChar(CP_UTF8, 0, tempBuf, -1, wszBuf, sizeof(wszBuf));
            OutputDebugStringW(wszBuf);
            WideCharToMultiByte(CP_ACP, 0, wszBuf, -1, tempBuf, sizeof(tempBuf), nullptr, FALSE);
            printf("%s", tempBuf);
            
            pos += MAX_LOG_LENGTH;
            
        } while (pos < len);
        SendLogToWindow(buf);
        fflush(stdout);
#else
        // Linux, Mac, iOS, etc
        fprintf(stdout, "%s", buf);
        fflush(stdout);
#endif
        
        //Director::getInstance()->getConsole()->log(buf);
        delete [] buf;
    }

    // FIXME: Deprecated
    void CCLog(const char * format, ...)
    {
        va_list args;
        va_start(args, format);
        _log(format, args);
        va_end(args);
    }

    void log(const char * format, ...)
    {
        va_list args;
        va_start(args, format);
        //_log(format, args);
        //printf(format, args);
        va_end(args);
    }
}

