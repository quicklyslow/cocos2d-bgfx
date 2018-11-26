#pragma once

#include "bx/spscqueue.h"

NS_CC_BEGIN

/** @brief This event system is designed to be used in a multi-threaded
 environment to communicated between two threads.
 Use this system as following.
 @example Communicate between threads.
 // Define a event queue.
 EventQueue _eventForOne;

 // Define worker functions.
 int threadOneFunc(void* userData)
 {
    while (true)
    {
        for (Own<QEvent> event = _eventForOne.poll();
            event != nullptr;
            event = _eventForOne.poll())
        {
            switch (Switch::hash(event->getName()))
            {
                case "Whatever"_hash:
                {
                    int val1, val2;
                    Slice msg;
                    event->get(val1, val2, msg);
                    Log("%d, %d, %s", val1, val2, msg);
                    break;
                }
            }
        }
    }
    return 0;
 }
 int threadTwoFunc(void* userData)
 {
    while (true)
    {
        _eventForOne.post("Whatever"_slice, 998, 233, "msg"_slice);
    }
    return 0;
 }

 // execute threads
 threadOne.init(threadOneFunc);
 threadTwo.init(threadTwoFunc);
 */

class QEvent
{
public:
    QEvent(String name);
    virtual ~QEvent();
    inline const std::string& getName() const { return name_; }

    template<typename... Args>
    void get(Args&... args);
protected:
    std::string name_;
    COCOS_TYPE_BASE(QEvent);
};

template<typename... Fields>
class QEventArgs : public QEvent
{
public:
    template<typename... Args>
    QEventArgs(String name, const Args&... args)
        :QEvent(name)
        ,arguments(std::make_tuple(args...))
    {}
    std::tuple<Fields...> arguments;
    COCOS_TYPE_OVERRIDE(QEventArgs<Fields...>);
};

template<typename... Args>
void QEvent::get(Args&... args)
{
    auto targetEvent = CocosCast<QEventArgs<Args...>>(this);
    CCAssertIf(targetEvent == nullptr, "no required event argument type can be retrieved.");
    std::tie(args...) = targetEvent->arguments;
}

class EventQueue
{
public:
    EventQueue();
    ~EventQueue();

    template<typename... Args>
    void post(String name, const Args& ...args)
    {
        auto event = new QEventArgs<Args...>(name, args...);
        queue_.push(event);
    }

    Own<QEvent> poll();

    QEvent* peek();
private:
    bx::DefaultAllocator allocator_;
    bx::SpScUnboundedQueueT<QEvent> queue_;
};

NS_CC_END