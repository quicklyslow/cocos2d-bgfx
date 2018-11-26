#pragma once

#include "EventQueue.h"
#include "Value.h"

NS_CC_BEGIN

class Async
{
    using Package = std::pair<std::function<SmartPtr<TValues>()>, std::function<void(TValues*)>>;
public:
    Async();
    virtual ~Async();
    void run(std::function<SmartPtr<TValues>()> worker, std::function<void(TValues*)> finisher);
    void run(std::function<void()> workder);
    void pause();
    void resume();
    void cancel();
    void stop();
    static int work(bx::Thread* thread, void* userData);
private:
    bool scheduled_;
    bx::Thread thread_;
    bx::Semaphore workerSemaphore_;
    bx::Semaphore pauseSemaphore_;
    std::vector<std::function<void()>*> workers_;
    std::vector<Package*> packages_;
    EventQueue workerEvent_;
    EventQueue finisherEvent_;
};

class AsyncThread
{
public:
    Async FileIO;
    Async Process;
    Async Loader;

    void stop();
#if BX_PLATFORM_WINDOWS
    inline void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }
    inline void operator delete(void* p)
    {
        _mm_free(p);
    }
#endif // BX_PLATFORM_WINDOWS
    SINGLETON_REF(AsyncThread);
};

#define SharedAsyncThread \
    cocos2d::Singleton<cocos2d::AsyncThread>::shared()

class AsyncLogThread : public Async
{
public:
    virtual ~AsyncLogThread()
    {
        Async::stop();
    }
#if BX_PLATFORM_WINDOWS
    inline void* operator new(size_t i)
    {
        return _mm_malloc(i, 16);
    }
    inline void operator delete(void* p)
    {
        _mm_free(p);
    }
#endif // BX_PLATFORM_WINDOWS
    SINGLETON_REF(AsyncLogThread);
};

#define SharedAsyncLogThread \
    cocos2d::Singleton<cocos2d::AsyncLogThread>::shared()

NS_CC_END