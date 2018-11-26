#pragma once

//#include <memory>
//#include <type_traits>

NS_CC_BEGIN

/** @brief Used with Composition Relationship. A std::unique_ptr wrapper*/
template<typename Item, class Del = std::default_delete<Item>>
class Own : public std::unique_ptr<Item, Del>
{
public:
    Own() {}
    Own(Own&& own):std::unique_ptr<Item>(std::move(own)) {}
    explicit Own(Item* item) :std::unique_ptr<Item>(item) {}

    inline operator Item* () const 
    {
        return std::unique_ptr<Item, Del>::get();
    }
    inline Own& operator=(std::nullptr_t)
    {
        std::unique_ptr<Item, Del>::reset();
        return (*this);
    }
    inline const Own& operator=(Own&& own)
    {
        std::unique_ptr<Item, Del>::operator=(std::move(own));
        return (*this);
    }
private:
    Own(const Own& own) = delete;
    const Own& operator=(const Own& own) = delete;
};

template<typename T>
inline Own<T> MakeOwn(T* item)
{
    return Own<T>(item);
}

template<typename T, class... Args>
inline Own<T> New(Args&&... args)
{
    return Own<T>(new T(std::forward<Args>(args)...));
}

NS_CC_END