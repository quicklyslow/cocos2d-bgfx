#include "ccHeader.h"
#include "CCApplicationProtocol.h"
#include "base/CCScriptSupport.h"
#include "base/CCAutoreleasePool.h"
#include "CCGLView.h"

NS_CC_BEGIN

ApplicationProtocol::~ApplicationProtocol()
{
#if CC_ENABLE_SCRIPT_BINDING
    ScriptEngineManager::destroyInstance();
#endif
    /** clean auto release pool. */
    PoolManager::destroyInstance();
}

NS_CC_END