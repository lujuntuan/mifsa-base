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

#ifndef MIFSA_BASE_QUEUE_H
#define MIFSA_BASE_QUEUE_H

#include "mifsa/base/define.h"
#include "mifsa/base/event.h"
#include "mifsa/base/timer.h"
#include <memory>
#include <mutex>

MIFSA_NAMESPACE_BEGIN

class MIFSA_EXPORT Queue {
    CLASS_DISSABLE_COPY_AND_ASSIGN(Queue)
public:
    explicit Queue(int queueId);
    virtual ~Queue();
    int run();
    void asyncRun();
    void quit(int quitCode = 0);
    void waitforQuit(uint32_t milli_s = 100);
    bool isRunning() const;
    bool isReadyToQuit() const;
    bool isBusy() const;
    bool isRunAsync() const;
    int quitCode() const;
    int eventCount() const;
    std::shared_ptr<Timer> createTimer(uint32_t interval_milli_s, bool loop = false, const Timer::Callback& cb = nullptr);
    void onceTimer(uint32_t interval_milli_s, const Timer::Callback& cb = nullptr);
    void postTask(const Timer::Callback& cb);
    void postEvent(const std::shared_ptr<Event>& event);
    void setMutex(std::mutex& mutex);
    void lock() const;
    void unlock() const;

protected:
    virtual void begin() { }
    virtual void end() { }
    virtual void eventChanged(const std::shared_ptr<Event>& event) = 0;
    virtual void processEvent();

private:
    void addTimer(const std::shared_ptr<Timer>& timer);
    void removeTimer(const std::shared_ptr<Timer>& timer);
    void removeTimer(Timer* timer);
    void wakeUpEvent();
    void processNextSleepTime();

private:
    friend Timer;
    struct QueueHelper* m_queueHelper = nullptr;
};

MIFSA_NAMESPACE_END

#endif // MIFSA_BASE_QUEUE_H
