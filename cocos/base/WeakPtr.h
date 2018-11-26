#pragma once

#include "base/CCRef.h"

NS_CC_BEGIN

template<typename T = Ref>
class WeakPtr
{
public:
    WeakPtr() : weak_(nullptr) {}
    explicit WeakPtr(T* item)
        :weak_(nullptr)
    {
        if (item)
        {
            weak_ = item->getWeakRef();
            weak_->retain();
        }
    }

    WeakPtr(const SmartPtr<T>& sp)
        :weak_(nullptr)
    {
        if (sp)
        {
            weak_ = sp->getWeakRef();
            weak_->retain();
        }
    }

    WeakPtr(const WeakPtr& wp)
        :weak_(wp.weak_)
    {
        if (weak_)
        {
            weak_->retain();
        }
    }

    WeakPtr(WeakPtr&& wrp)
        :weak_(wrp.weak_)
    {
        wrp.weak_ = nullptr;
    }

    ~WeakPtr()
    {
        if (weak_)
        {
            weak_->release();
        }
    }

    inline T* operator->() const
    {
        return get();
    }

    T* operator=(T* item)
    {
        Weak* weak = nullptr;
        if (item)
        {
            weak = item->getWeakRef();
            weak->retain();
        }

        if (weak_)
        {
            weak_->release();
        }
        weak_ = weak;
        return item;
    }

    const WeakPtr& operator=(const SmartPtr<T>& sp)
    {
        operator=(sp.get());
        return *this;
    }

    const WeakPtr& operator=(const WeakPtr& wp)
    {
        operator=(wp.get());
        return *this;
    }

    const WeakPtr& operator=(WeakPtr&& wrp)
    {
        if (this == &wrp)
        {
            return *this;
        }
        if (weak_)
        {
            weak_->release();
        }
        weak_ = wrp.weak_;
        wrp.weak_ = nullptr;
        return *this;
    }

    bool operator==(const WeakPtr& wp) const
    {
        return get() == wp.get();
    }

    bool operator!=(const WeakPtr& wp) const
    {
        return !operator==(wp);
    }

    inline operator T*() const
    {
        return get();
    }

    inline T* get() const
    {
        if (weak_)
            return static_cast<T*>(weak_->target);
        else
            return nullptr;
    }

private:
    Weak* weak_;
};

template<typename T>
inline WeakPtr<T> MakeWeakPtr(T* item)
{
    return WeakPtr<T>(item);
}

NS_CC_END