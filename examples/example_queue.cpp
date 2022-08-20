/*********************************************************************************
 *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
 *Author:  Juntuan.Lu
 *Version: 1.0
 *Date:  2022/04/01
 *Email: 931852884@qq.com
 *Description:
 *Others:
 *Function List:
 *History:
 **********************************************************************************/

#include <mifsa/base/application.h>
#include <mifsa/base/queue.h>
#include <mifsa/base/singleton.h>
#include <mifsa/base/thread.h>

using namespace Mifsa;

#define DEMO_QUEUE_NUM1 1
#define DEMO_QUEUE_NUM2 2

class DemoApplication;

class FirstEvent : public Event {
public:
    enum Type {
        FIRST_EVENT_1 = 0,
        FIRST_EVENT_2,
        FIRST_EVENT_3
    };
    FirstEvent(int type = UNKNOWN, const Callback& cb = nullptr)
        : Event(DEMO_QUEUE_NUM1, type, cb)
    {
    }

private:
    VariantMap m_data;
};
class SecondEvent : public Event {
public:
    enum Type {
        SECOND_EVENT_1 = 0,
        SECOND_EVENT_2,
        SECOND_EVENT_3
    };
    SecondEvent(int type = UNKNOWN, const Callback& cb = nullptr)
        : Event(DEMO_QUEUE_NUM2, type, cb)
    {
    }
};

class FirstQueue : public Queue {
public:
    FirstQueue()
        : Queue(DEMO_QUEUE_NUM1)
    {
    }
    ~FirstQueue()
    {
    }

protected:
    void eventChanged(const std::shared_ptr<Event>& event) override
    {
        auto firstEvent = std::static_pointer_cast<FirstEvent>(event);
        if (firstEvent->isAccepted()) {
            return;
        }
        switch (firstEvent->type()) {
        case FirstEvent::FIRST_EVENT_1: {
            LOG_DEBUG("rev:FIRST_EVENT_1");
            break;
        }
        case FirstEvent::FIRST_EVENT_2: {
            LOG_DEBUG("rev:FIRST_EVENT_2");
            break;
        }
        case FirstEvent::FIRST_EVENT_3: {
            LOG_DEBUG("rev:FIRST_EVENT_3");
            break;
        }
        }
    }
};

class SecondQueue : public Queue {
public:
    SecondQueue()
        : Queue(DEMO_QUEUE_NUM2)
    {
    }
    ~SecondQueue()
    {
    }

protected:
    void eventChanged(const std::shared_ptr<Event>& event) override
    {
        auto firstEvent = std::static_pointer_cast<FirstEvent>(event);
        if (firstEvent->isAccepted()) {
            return;
        }
        switch (firstEvent->type()) {
        case SecondEvent::SECOND_EVENT_1: {
            LOG_DEBUG("rev:SECOND_EVENT_1");
            break;
        }
        case SecondEvent::SECOND_EVENT_2: {
            LOG_DEBUG("rev:SECOND_EVENT_2");
            break;
        }
        case SecondEvent::SECOND_EVENT_3: {
            LOG_DEBUG("rev:SECOND_EVENT_3");
            break;
        }
        }
    }
};

class DemoApplication : public Application, public SingletonProxy<DemoApplication> {
public:
    DemoApplication(int argc, char** argv, const std::string& configPath = "")
        : Application(argc, argv, configPath)
    {
    }
    ~DemoApplication()
    {
    }
    virtual int exec(int flag = CHECK_SINGLETON | CHECK_TERMINATE) override
    {
        this->parserFlag(flag);
        _firstQueue.asyncRun();
        _secondQueue.run();
        return 0;
    }
    virtual void exit(int exitCode = 0) override
    {
        _firstQueue.quit(exitCode);
        _secondQueue.quit(exitCode);
    }

public:
    FirstQueue _firstQueue;
    SecondQueue _secondQueue;
};

int main(int argc, char* argv[])
{
    DemoApplication app(argc, argv);
    auto testCb1 = [&]() {
        LOG_DEBUG("thread-id1:", Thread::currentThreadId());
        app._firstQueue.postEvent(std::make_shared<FirstEvent>(FirstEvent::FIRST_EVENT_2));
    };
    auto testCb2 = [&]() {
        LOG_DEBUG("thread-id2:", Thread::currentThreadId());
        app._secondQueue.postEvent(std::make_shared<SecondEvent>(SecondEvent::SECOND_EVENT_2));
    };
    app._secondQueue.postTask([]() {
        LOG_DEBUG("[task] thread-id3:", Thread::currentThreadId());
    });
    app._firstQueue.postEvent(std::make_shared<FirstEvent>(FirstEvent::FIRST_EVENT_1, testCb1));
    app._secondQueue.postEvent(std::make_shared<SecondEvent>(SecondEvent::SECOND_EVENT_1, testCb2));
    return app.exec();
}
