#include "ccHeader.h"
#include "Async.h"
#include "base/CCDirector.h"
#include "base/CCScheduler.h"

NS_CC_BEGIN

Async::Async()
    :scheduled_(false)
{

}

Async::~Async()
{
    if (thread_.isRunning())
    {
        Async::cancel();
        Async::stop();
    }
}

void Async::stop()
{
    if (thread_.isRunning())
    {
        workerEvent_.post("Stop"_slice);
        workerSemaphore_.post();
        thread_.shutdown();
    }
    scheduled_ = false;
}

void Async::run(std::function<SmartPtr<TValues>()> worker, std::function<void(TValues*)> finisher)
{
    if (!thread_.isRunning())
    {
        thread_.init(Async::work, this);
    }
    if (!scheduled_)
    {
        scheduled_ = true;
        SharedDirector.getScheduler()->schedule([this](float deltaTime) -> bool
        {
            for (Own<QEvent> event = finisherEvent_.poll();
                event != nullptr;
                event = finisherEvent_.poll())
            {
                Package* package;
                SmartPtr<TValues> result;
                event->get(package, result);
                package->second(result);
                delete package;
            }
            return false;
        });
    }
    auto package = new Package(worker, finisher);
    workerEvent_.post("WorkDone"_slice, package);
    workerSemaphore_.post();
}

void Async::run(std::function<void()> workder)
{
    if (!thread_.isRunning())
    {
        thread_.init(Async::work, this);
    }
    auto work = new std::function<void()>(workder);
    workerEvent_.post("Work"_slice, work);
    workerSemaphore_.post();
}

int Async::work(bx::Thread* thread, void* userData)
{
    Async* worker = reinterpret_cast<Async*>(userData);
    while (true)
    {
        for (Own<QEvent> event = worker->workerEvent_.poll();
            event != nullptr;
            event = worker->workerEvent_.poll())
        {
            switch (Switch::hash(event->getName()))
            {
            case "Work"_hash:
            {
                std::function<void()>* worker;
                event->get(worker);
                (*worker)();
                delete worker;
                break;
            }
            case "WorkDone"_hash:
            {
                Package* package;
                event->get(package);
                SmartPtr<TValues> result = package->first();
                worker->finisherEvent_.post(Slice::Empty, package, result);
                break;
            }
            case "Stop"_hash:
            {
                return 0;
            }
            }
        }
        worker->pauseSemaphore_.post();
        worker->workerSemaphore_.wait();
    }
    return 0;
}

void Async::pause()
{
    if (thread_.isRunning())
    {
        for (Own<QEvent> event = workerEvent_.poll();
            event != nullptr;
            event = workerEvent_.poll())
        {
            switch (Switch::hash(event->getName()))
            {
            case "Work"_hash:
            {
                std::function<void()>* worker;
                event->get(worker);
                workers_.push_back(worker);
                break;
            }
            case "WorkDone"_hash:
            {
                Package* package;
                event->get(package);
                packages_.push_back(package);
                break;
            }
            }
        }
        workerSemaphore_.post();
        pauseSemaphore_.wait();
    }
}

void Async::resume()
{
    if (thread_.isRunning() && !packages_.empty())
    {
        for (Package* package : packages_)
        {
            workerEvent_.post("WorkDone"_slice, package);
        }
        for (std::function<void()>* worker : workers_)
        {
            workerEvent_.post("Work"_slice, worker);
        }
        packages_.clear();
        workers_.clear();
        workerSemaphore_.post();
    }
}

void Async::cancel()
{
    for (Own<QEvent> event = workerEvent_.poll();
        event != nullptr;
        event = workerEvent_.poll())
    {
        switch (Switch::hash(event->getName()))
        {
        case "Work"_hash:
        {
            std::function<void()>* worker;
            event->get(worker);
            delete worker;
            break;
        }
        case "WorkDone"_hash:
        {
            Package* package;
            event->get(package);
            delete package;
            break;
        }
        }
    }
    for (Package* package : packages_)
    {
        delete package;
    }
    packages_.clear();
    for (std::function<void()>* worker : workers_)
    {
        delete worker;
    }
    workers_.clear();
}

void AsyncThread::stop()
{
    FileIO.stop();
    Process.stop();
    Loader.stop();
}

NS_CC_END