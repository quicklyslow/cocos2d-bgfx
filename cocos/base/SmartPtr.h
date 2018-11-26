#pragma once

#include "base/CCRef.h"

NS_CC_BEGIN

template<typename T = Ref>
class SmartPtr
{
public:
    SmartPtr(): item_(nullptr){}
    explicit SmartPtr(T* item) :item_(item)
    {
        if (item)
        {
            item->retain();
        }
    }
    SmartPtr(const SmartPtr& ptr)
    {
        if (ptr.item_)
        {
            ptr.item_->retain();
        }
        item_ = ptr.item_;
    }
    SmartPtr(SmartPtr&& ptr)
    {
        item_ = ptr.item_;
        ptr.item_ = nullptr;
    }
    ~SmartPtr()
    {
        if (item_)
        {
            item_->release();
            item_ = nullptr;
        }
    }
    inline T* operator->() const
    {
        return static_cast<T*>(item_);
    }
    //from raw pointer to this smartptr
    T* operator=(T* item)
    {
        if (item)
        {
            item->retain();
        }
        if (item_)
        {
            item_->release();
        }
        item_ = item;
        return item;
    }
    const SmartPtr& operator=(const SmartPtr& ptr)
    {
        if (this == &ptr)
        {
            return *this;
        }
        if (ptr.item_)
        {
            ptr.item_->retain();
        }
        if (item_)
        {
            item_->release();
        }
        item_ = ptr.item_;
        return *this;
    }
    const SmartPtr& operator=(SmartPtr&& ptr)
    {
        if (this == &ptr)
        {
            return *this;
        }
        if (item_)
        {
            item_->release();
        }
        item_ = ptr.item_;
        ptr.item_ = nullptr;
        return *this;
    }
    bool operator==(const SmartPtr& ptr) const
    {
        return item_ == ptr.item_;
    }
    bool operator!=(const SmartPtr& ptr) const
    {
        return item_ != ptr.item_;
    }
    inline operator T*() const
    {
        return reinterpret_cast<T*>(item_);
    }
    inline T* get() const
    {
        return reinterpret_cast<T*>(item_);
    }
    template<typename Type>
    inline Type* to() const
    {
        return static_cast<Type*>(item_);
    }
private:
    Ref * item_;
};

NS_CC_END
