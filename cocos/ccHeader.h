#pragma once
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <string>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <memory>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdint>

//this is all about define _WIN32_WINNT in bx/platform.h
//it should be included before bx.h
//or else the inet_pton in FileServer.cpp will not be defined
//if I put #include <WS2tcpip.h> here, the max/min macro is crazy
//libsimulator is such a mess!
#if defined(_MSC_VER) 
#include <sdkddkver.h>
#endif

#include "base/Slice.h"
using namespace silly::slice;
#include "bgfx/platform.h"
#include "bgfx/bgfx.h"
#include "bgfx/embedded_shader.h"
#include "bx/thread.h"
#include "bx/math.h"
#include "base/CCLog.h"
#include "base/ccMacros.h"
#include "base/ccTypes.h"
using namespace cocos2d::Switch::Literals;
#include "base/ccConfig.h"
#include "base/Singleton.h"
#include "base/Own.h"
#include "base/CCRef.h"
#include "base/SmartPtr.h"
#include "base/WeakPtr.h"
#include "math/CCMath.h"
#include "math/CCGeometry.h"

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <io.h>
#include <WS2tcpip.h>
#include <Winsock2.h>
#endif
