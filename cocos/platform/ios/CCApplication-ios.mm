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
#import "CCApplication.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#import <UIKit/UIKit.h>
#import <AdSupport/ASIdentifierManager.h>

#import "math/CCGeometry.h"
#import "base/CCDirector.h"
#import "platform/ios/CCDirectorCaller-ios.h"

#include <sys/socket.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>

#import "OpenUDID.h"

#import <mach/mach.h>

#include "bgfx/bgfx.h"

#if BX_PLATFORM_OSX || BX_PLATFORM_IOS
NS_CC_BEGIN
bgfx::RenderFrame::Enum Application::renderFrame()
{
	bgfx::RenderFrame::Enum result;
	@autoreleasepool
	{
		result = bgfx::renderFrame();
	}
	return result;
}
NS_CC_END
#endif // BX_PLATFORM_OSX || BX_PLATFORM_IOS

NS_CC_BEGIN

 int Application::run()
 {
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
     
     {
         [[CCDirectorCaller sharedDirectorCaller] startMainLoop];
     }
     return 0;
 }


#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

void Application::setAnimationInterval(float interval)
{
    //[[CCDirectorCaller sharedDirectorCaller] setAnimationInterval: interval ];
}


const char * Application::getCurrentLanguageCode()
{
    static char code[3]={0};
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
    NSString *currentLanguage = [languages objectAtIndex:0];

    // get the current language code.(such as English is "en", Chinese is "zh" and so on)
    NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
    NSString * languageCode = [temp objectForKey:NSLocaleLanguageCode];
    [languageCode getCString:code maxLength:3 encoding:NSASCIIStringEncoding];
    code[2]='\0';
    return code;
}

LanguageType Application::getCurrentLanguage()
{
    // get the current language and country config
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *languages = [defaults objectForKey:@"AppleLanguages"];
    NSString *currentLanguage = [languages objectAtIndex:0];

    // get the current language code.(such as English is "en", Chinese is "zh" and so on)
    NSDictionary* temp = [NSLocale componentsFromLocaleIdentifier:currentLanguage];
    NSString * languageCode = [temp objectForKey:NSLocaleLanguageCode];

    if ([languageCode isEqualToString:@"zh"]) return LanguageType::CHINESE;
    if ([languageCode isEqualToString:@"en"]) return LanguageType::ENGLISH;
    if ([languageCode isEqualToString:@"fr"]) return LanguageType::FRENCH;
    if ([languageCode isEqualToString:@"it"]) return LanguageType::ITALIAN;
    if ([languageCode isEqualToString:@"de"]) return LanguageType::GERMAN;
    if ([languageCode isEqualToString:@"es"]) return LanguageType::SPANISH;
    if ([languageCode isEqualToString:@"nl"]) return LanguageType::DUTCH;
    if ([languageCode isEqualToString:@"ru"]) return LanguageType::RUSSIAN;
    if ([languageCode isEqualToString:@"ko"]) return LanguageType::KOREAN;
    if ([languageCode isEqualToString:@"ja"]) return LanguageType::JAPANESE;
    if ([languageCode isEqualToString:@"hu"]) return LanguageType::HUNGARIAN;
    if ([languageCode isEqualToString:@"pt"]) return LanguageType::PORTUGUESE;
    if ([languageCode isEqualToString:@"ar"]) return LanguageType::ARABIC;
    if ([languageCode isEqualToString:@"nb"]) return LanguageType::NORWEGIAN;
    if ([languageCode isEqualToString:@"pl"]) return LanguageType::POLISH;
    if ([languageCode isEqualToString:@"tr"]) return LanguageType::TURKISH;
    if ([languageCode isEqualToString:@"uk"]) return LanguageType::UKRAINIAN;
    if ([languageCode isEqualToString:@"ro"]) return LanguageType::ROMANIAN;
    if ([languageCode isEqualToString:@"bg"]) return LanguageType::BULGARIAN;
    return LanguageType::ENGLISH;

}

Application::Platform Application::getTargetPlatform()
{
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) // idiom for iOS <= 3.2, otherwise: [UIDevice userInterfaceIdiom] is faster.
    {
        return Platform::OS_IPAD;
    }
    else
    {
        return Platform::OS_IPHONE;
    }
}

std::string Application::getVersion() {
    NSString* version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    if (version) {
        return [version UTF8String];
    }
    return "";
}

bool Application::openURL(const std::string &url)
{
    NSString* msg = [NSString stringWithCString:url.c_str() encoding:NSUTF8StringEncoding];
    NSURL* nsUrl = [NSURL URLWithString:msg];
    return [[UIApplication sharedApplication] openURL:nsUrl];
}

const char* Application::getMacAddress()
{
    if (m_macAddress.length() != 0) {
        return m_macAddress.c_str();
    } else {
        int mib[6];
        size_t len;
        char *buf;
        unsigned char *ptr;
        struct if_msghdr *ifm;
        struct sockaddr_dl *sdl;

        mib[0] = CTL_NET;
        mib[1] = AF_ROUTE;
        mib[2] = 0;
        mib[3] = AF_LINK;
        mib[4] = NET_RT_IFLIST;

        if ((mib[5] = if_nametoindex("en0")) == 0) {
            return m_macAddress.c_str();
        }
        if (sysctl(mib, 6, NULL, &len, NULL, 0)) {
            return m_macAddress.c_str();
        }
        if ((buf = (char*)malloc(len)) == NULL) {
            return m_macAddress.c_str();
        }
        if (sysctl(mib, 6, buf, &len, NULL, 0) < 0) {
            return m_macAddress.c_str();
        }

        ifm = (struct if_msghdr *)buf;
        sdl = (struct sockaddr_dl *)(ifm + 1);
        ptr = (unsigned char*)LLADDR(sdl);
        free(buf);
        NSString *outstring = [NSString stringWithFormat:@"%02X%02X%02X%02X%02X%02X", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5)];
        m_macAddress = [outstring UTF8String];
        return m_macAddress.c_str();
    }
}

const char* Application::getAdvertisingIdentifier()
{
    if (m_advertisingIdentifier.length() != 0) {
        return m_advertisingIdentifier.c_str();
    } else {
        NSString *adId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
        if (adId == nil) {
            return m_advertisingIdentifier.c_str();
        }
        m_advertisingIdentifier = [adId UTF8String];
        return m_advertisingIdentifier.c_str();
    }
}

const char* Application::getOpenUDID()
{
    if (m_openUDID.length() != 0) {
        return m_openUDID.c_str();
    } else {
        NSString *openUDID = [OpenUDID value];
        if (openUDID != nil) {
            m_openUDID = [openUDID UTF8String];
        }
        return m_openUDID.c_str();
    }
}

const char* Application::getIMEI()
{
    return nullptr;
}

const char* Application::getUDID()
{
#ifdef APPLE
    return m_udid.c_str();
#else
    if(m_udid.length() != 0) {
        return m_udid.c_str();
    } else {
        NSString *uuid = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
        if (uuid != nil) {
            m_udid = [uuid UTF8String];
        }
        return m_udid.c_str();
    }
#endif
}

int Application::getAvailableMemory()
{
    vm_statistics_data_t vmStats;
    mach_msg_type_number_t infoCount = HOST_VM_INFO_COUNT;
    kern_return_t kernReturn = host_statistics(mach_host_self(),
                                                HOST_VM_INFO,
                                                (host_info_t)&vmStats,
                                                &infoCount);

    if (kernReturn != KERN_SUCCESS) {
        return NSNotFound;
    }

    return ((vm_page_size *vmStats.free_count) / 1024.0) / 1024.0;
}
#endif

NS_CC_END

#endif // CC_PLATFORM_IOS
