#include "ccHeader.h"
#include "EventQueue.h"

NS_CC_BEGIN

QEvent::QEvent(String name)
:name_(name)
{
}

QEvent::~QEvent()
{
}

EventQueue::EventQueue()
    :queue_(&allocator_)
{

}

EventQueue::~EventQueue()
{
    for (QEvent* e = poll(); e; e = poll());
}

Own<QEvent> EventQueue::poll()
{
    return MakeOwn(queue_.pop());
}

QEvent* EventQueue::peek()
{
    return queue_.peek();
}


NS_CC_END
