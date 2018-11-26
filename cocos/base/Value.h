#pragma once

NS_CC_BEGIN

template<typename T, typename Enable = void>
class TValueEx;

class TValue : public Ref
{
public:
    template<typename T>
    const T& to();

    template<typename T>
    TValueEx<T>* as();

    template<typename T>
    static TValue* create(const T& value);
protected:
    TValue() {}
};

template<typename T>
class TValueEx<T, typename std::enable_if<!std::is_base_of<Ref, T>::value>::type> : public TValue
{
public:
    inline void set(const T& value)
    {
        value_ = value;
    }
    inline T& get()
    {
        return value_;
    }
    CREATE_FUNC(TValueEx<T>);
protected:
    TValueEx(const T& value)
        : value_(value)
    {}
    TValueEx(T&& value)
        : value_(std::move(value))
    {}
private:
    T value_;
    COCOS_TYPE_OVERRIDE(TValueEx<T>);
};

template<>
class TValueEx<Ref*> : public TValue
{
public:
    inline void set(Ref* value)
    {
        value_ = value;
    }
    inline Ref* get()
    {
        return value_.get();
    }
    CREATE_FUNC(TValueEx<Ref*>);
private:
    SmartPtr<> value_;
    COCOS_TYPE_OVERRIDE(TValueEx<Ref*>);
};

template<typename T>
const T& TValue::to()
{
    TValueEx<T>* item = CocosCast<TValueEx<T>>(this);
    CCASSERT(item != nullptr, "can not get value of target type from value object");
    return item->get();
}

template<typename T>
TValueEx<T>* TValue::as()
{
    return CocosCast<TValueEx<T>>(this);
}

template<typename T>
TValue* TValue::create(const T& value)
{
    return TValueEx<T>::create(value);
}

class TValues : public Ref
{
public:
    virtual ~TValues() {}
    template<typename... Args>
    static SmartPtr<TValues> create(const Args&... args);
    template<typename... Args>
    void get(Args&... args);
    static const SmartPtr<TValues> None;
protected:
    TValues() { }
};

template<typename... Fields>
class TValuesEx : public TValues
{
public:
    template<typename... Args>
    TValuesEx(const Args&... args)
        :values(std::make_tuple(args...))
    { }
    std::tuple<Fields...> values;
    COCOS_TYPE_OVERRIDE(TValuesEx<Fields...>);
};

template<typename... Args>
SmartPtr<TValues> TValues::create(const Args&... args)
{
    auto item = new TValuesEx<Args...>(args...);
    SmartPtr<TValues> itemRef(item);
    item->release();
    return itemRef;
}

template<typename... Args>
void TValues::get(Args&... args)
{
    auto values = CocosCast<TValuesEx<Args...>>(this);
    CCAssertIf(values == nullptr, "no required value type can be retrieved.");
    std::tie(args...) = values->values;
}

NS_CC_END