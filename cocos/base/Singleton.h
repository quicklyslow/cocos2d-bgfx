#pragma once

#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

class Life
{
public:
    virtual ~Life() {}
    static void addDependency(String target, String dependency);
    static void addItem(String name, Life* life);
    static void addName(String name);
    static void destroy(String name);
#if COCOS2D_DEBUG
    static void assertIf(bool disposed, String name);
#endif
};

template<typename T>
class Singleton : public T, public Life
{
public:
    Singleton()
    {
        disposed_ = false;
        Life::addItem(name_, getLife());
    }

    static Singleton& shared()
    {
        static auto* instance_ = new Singleton();
#if COCOS2D_DEBUG
        Life::assertIf(disposed_, name_);
#endif
        return *instance_;
    }

    static Singleton* getInstance()
    {
        return &shared();
    }

    virtual ~Singleton()
    {
        disposed_ = true;
    }

    const std::string& getName() const
    {
        return name_;
    }

    T* getTarget() const
    {
        return static_cast<T*>(const_cast<Singleton*>(this));
    }

    Life* getLife() const
    {
        return static_cast<Life*>(const_cast<Singleton*>(this));
    }

    static bool isDisposed()
    {
        return disposed_;
    }

    static void setDependencyInfo(String name, String dependencyStr)
    {
        name_ = name;
        Life::addName(name);
        auto dependencies = dependencyStr.split(",");
        for (auto& dependency : dependencies)
        {
            dependency.trimSpace();
            Life::addDependency(name, dependency);
        }
    }
private:
    static std::string name_;
    static bool disposed_;
};

template<typename T>
std::string Singleton<T>::name_;

template<typename T>
bool Singleton<T>::disposed_ = true;

#define SINGLETON_REF(type, ...) \
private: \
    struct type##_ref_initializer \
    {\
        type##_ref_initializer() \
        {\
            const char* info[] = {nullptr, #__VA_ARGS__}; \
            Singleton<type>::setDependencyInfo(#type, (sizeof(info)/sizeof(*info) == 1 ? "" : info[1])); \
        }\
    } __##type##_initializer__

NS_CC_END
