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
#include <mifsa/base/thread.h>

using namespace Mifsa;

#define DEMO_QUEUE_NUM1 1
#define DEMO_QUEUE_NUM2 2

class FirstEvent : public Event {
public:
    enum Type {
        FIRST_EVENT_1 = 0,
        FIRST_EVENT_2,
        FIRST_EVENT_3
    };
    FirstEvent(int type = UNKNOWN, const VariantMap& data = {}, const Callback& cb = nullptr)
        : Event(DEMO_QUEUE_NUM1, type, cb)
        , m_data(data)
    {
    }

public:
    inline VariantMap data() const
    {
        return m_data;
    };
    void setData(const VariantMap& data)
    {
        m_data = data;
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

class DemoApplication : public Application, public Queue {
public:
    DemoApplication(int argc, char** argv, const std::string& configPath = "")
        : Application(argc, argv, configPath)
        , Queue(DEMO_QUEUE_NUM1)
    {
    }
    ~DemoApplication()
    {
    }
    virtual int exec(int flag = CHECK_SINGLETON | CHECK_TERMINATE) override
    {
        this->parserFlag(flag);
        return Queue::run();
    }
    virtual void exit(int exitCode = 0) override
    {
        Queue::quit(exitCode);
    }
    virtual void eventChanged(const std::shared_ptr<Event>& event) override
    {
        LOG_DEBUG("thread-id3:", Thread::currentThreadId());
        auto firstEvent = std::static_pointer_cast<FirstEvent>(event);
        if (firstEvent->isAccepted()) {
            return;
        }
        switch (firstEvent->type()) {
        case FirstEvent::FIRST_EVENT_1: {
            LOG_PROPERTY("event1", firstEvent->data());
            VariantMap data = firstEvent->data();
            data.insert("temperature", 38);
            data.insert("sex", "man");
            postEvent(std::make_shared<FirstEvent>(FirstEvent::FIRST_EVENT_2, data));
            break;
        }
        case FirstEvent::FIRST_EVENT_2: {
            LOG_PROPERTY("event2", firstEvent->data());
            VariantMap data;
            data.insert("error", "error");
            postEvent(std::make_shared<FirstEvent>(FirstEvent::FIRST_EVENT_3, data));
            break;
        }
        case FirstEvent::FIRST_EVENT_3: {
            LOG_PROPERTY("event3", firstEvent->data());
            // test error
            postEvent(std::make_shared<SecondEvent>(SecondEvent::SECOND_EVENT_2));
            break;
        }
        default:
            break;
        }
    }
    virtual void begin() override
    {
        LOG_DEBUG("begin");
    }
    virtual void end() override
    {
        LOG_DEBUG("end");
    }
};

int main(int argc, char* argv[])
{
    DemoApplication app(argc, argv);
    VariantMap data = {
        { "name", "zhangsan" },
        { "arg", 18 },
        { "phone", "1234567" },
        { "temperature", 37.8f },
        { "address", VariantList { "home", "school" } },
        { "other", VariantList { 1.0, 2.0 } },
    };
    LOG_DEBUG("thread-id1:", Thread::currentThreadId());
    auto testCb = [&]() {
        LOG_DEBUG("thread-id2:", Thread::currentThreadId());
    };
    app.postEvent(std::make_shared<FirstEvent>(FirstEvent::FIRST_EVENT_1, data, testCb));
    return app.exec();
}
