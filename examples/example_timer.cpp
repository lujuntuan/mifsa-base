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

#include <thread>
#include <mifsa/base/application.h>
#include <mifsa/base/elapsed.h>
#include <mifsa/base/queue.h>
#include <mifsa/base/timer.h>

using namespace Mifsa;

#define DEMO_QUEUE_NUM1 1

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
        LOG_DEBUG(11);
        Queue::quit(exitCode);
    }
    virtual void eventChanged(const std::shared_ptr<Event>& event) override
    {
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
    Elapsed elapsed;
    elapsed.start();
    app.onceTimer(5000, [&]() {
        LOG_DEBUG("once time:", elapsed.get());
    });
    auto timer = app.createTimer(
        500, true, [&]() {
            LOG_DEBUG("time:", elapsed.get());
        });
    timer->start();
    return app.exec();
}
