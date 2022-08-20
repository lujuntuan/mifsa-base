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

#include "mifsa/base/queue.h"
#include "mifsa/base/log.h"
#include "mifsa/base/semaphore.h"
#include "mifsa/base/thread.h"
#include "mifsa/utils/time.h"
#include <iostream>
#include <list>
#include <queue>
#include <shared_mutex>
#include <thread>

#define QUEUE_MAX_NUM 4096

#define m_hpr m_queueHelper

MIFSA_NAMESPACE_BEGIN

struct TimeMethod {
    int64_t miniTime = 0;
    int64_t remainTime = 0;
    int64_t intervalTime = 0;
    uint64_t currentTime = 0;
#if TIMER_USE_PRECISION
    uint64_t expactCount = 0;
#endif
};

struct QueueHelper {
    int queueId = -1;
    std::atomic_bool isRunning { false };
    std::atomic_bool quitFlag { false };
    std::atomic_bool isBusy { false };
    std::atomic_int quitCode { 0 };
    std::queue<std::shared_ptr<Event>> eventQueue;
    std::queue<Timer::Callback> taskQueue;
    std::list<std::shared_ptr<Timer>> timerList;
    std::mutex eventMutex;
    std::thread::id workThreadId;
    Semaphore eventSemaphore { 0 };
    Semaphore quitSemaphore { 0 };
    std::unique_ptr<Thread> queueThread;
    TimeMethod timeMethod;
    std::mutex* commonMutex = nullptr;
};

Queue::Queue(int queueId)
{
    if (queueId < 0) {
        LOG_WARNING("queue id not set");
    }
    MIFSA_HELPER_CREATE(m_hpr);
    m_hpr->queueId = queueId;
}

Queue::~Queue()
{
    m_hpr->eventSemaphore.reset();
    m_hpr->quitSemaphore.reset();
    //
    m_hpr->eventMutex.lock();
    for (; !m_hpr->eventQueue.empty();) {
        m_hpr->eventQueue.pop();
    }
    for (const auto& timer : m_hpr->timerList) {
        if (timer) {
            timer->m_queue = nullptr;
        }
    }
    m_hpr->eventMutex.unlock();
    // m_hpr->queueThread.reset();
    MIFSA_HELPER_DESTROY(m_hpr);
}

int Queue::run()
{
    m_hpr->workThreadId = std::this_thread::get_id();
    if (m_hpr->isRunning && !m_hpr->queueThread) {
        LOG_WARNING("queue has run");
        return -1;
    }
    m_hpr->isRunning = true;
    if (m_hpr->quitFlag) {
        return m_hpr->quitCode;
    }
    begin();
    for (; !m_hpr->quitFlag;) {
        processEvent();
        if (m_hpr->quitFlag) {
            break;
        }
        processNextSleepTime();
        m_hpr->eventSemaphore.acquire_nano(m_hpr->timeMethod.miniTime);
    }
    processEvent();
    end();
    m_hpr->isRunning = false;
    m_hpr->quitSemaphore.reset();
    return m_hpr->quitCode;
}

void Queue::asyncRun()
{
    if (m_hpr->isRunning || m_hpr->queueThread) {
        LOG_WARNING("queue has run");
        return;
    }
    m_hpr->isRunning = true;
    m_hpr->queueThread = std::make_unique<Thread>([this]() {
        run();
    });
    m_hpr->queueThread->start();
}

void Queue::quit(int quitCode)
{
    if (m_hpr->quitFlag) {
        return;
    }
    m_hpr->quitFlag = true;
    m_hpr->quitCode = quitCode;
    m_hpr->eventSemaphore.reset();
    if (m_hpr->queueThread) {
        m_hpr->queueThread->stop();
    }
}

void Queue::waitforQuit(uint32_t milli_s)
{
    if (std::this_thread::get_id() == m_hpr->workThreadId) {
        LOG_WARNING("can not wait in same thread");
    } else {
        if (m_hpr->queueThread) {
            m_hpr->queueThread->stop(milli_s);
        } else {
            m_hpr->quitSemaphore.reset();
            int32_t time = milli_s;
            if (time <= 0) {
                time = -1;
            }
            if (m_hpr->quitSemaphore.acquire(time) == false) {
                LOG_WARNING("wait time out");
            }
        }
    }
}

bool Queue::isRunning() const
{
    return m_hpr->isRunning;
}

bool Queue::isReadyToQuit() const
{
    return m_hpr->quitFlag;
}

bool Queue::isBusy() const
{
    return m_hpr->isBusy;
}

bool Queue::isRunAsync() const
{
    if (m_hpr->queueThread) {
        return true;
    }
    return false;
}

int Queue::quitCode() const
{
    return m_hpr->quitCode;
}

int Queue::eventCount() const
{
    m_hpr->eventMutex.lock();
    int count = (int)m_hpr->eventQueue.size();
    m_hpr->eventMutex.unlock();
    return count;
}

std::shared_ptr<Timer> Queue::createTimer(uint32_t interval_milli_s, bool loop, const Timer::Callback& cb)
{
    return Timer::create(this, interval_milli_s, loop, cb);
}

void Queue::onceTimer(uint32_t interval_milli_s, const Timer::Callback& cb)
{
    std::weak_ptr<Timer> timer = createTimer(interval_milli_s, false, nullptr);
    timer.lock()->m_cb = [timer, cb, this] {
        if (cb) {
            cb();
            removeTimer(timer.lock());
        }
    };
    timer.lock()->start();
}

void Queue::postTask(const Timer::Callback& cb)
{
    m_hpr->eventMutex.lock();
    if (m_hpr->taskQueue.size() > QUEUE_MAX_NUM) {
        LOG_WARNING("task queue is full");
        std::terminate();
    }
    m_hpr->taskQueue.push(cb);
    m_hpr->eventMutex.unlock();
    m_hpr->eventSemaphore.reset();
}

void Queue::postEvent(const std::shared_ptr<Event>& event)
{
    if (event->queueId() < 0) {
        LOG_WARNING("queue id not set");
        return;
    } else {
        if (event->queueId() != m_hpr->queueId) {
            LOG_WARNING("queue not equal target id");
            return;
        }
    }
    m_hpr->eventMutex.lock();
    if (m_hpr->eventQueue.size() > QUEUE_MAX_NUM) {
        LOG_WARNING("event queue is full");
        std::terminate();
    }
    m_hpr->eventQueue.push(event);
    m_hpr->eventMutex.unlock();
    m_hpr->eventSemaphore.reset();
}

void Queue::setMutex(std::mutex& mutex)
{
    m_hpr->commonMutex = &mutex;
}

void Queue::lock() const
{
    if (m_hpr->commonMutex) {
        m_hpr->commonMutex->lock();
    } else {
        LOG_WARNING("mutex not set");
    }
}

void Queue::unlock() const
{
    if (m_hpr->commonMutex) {
        m_hpr->commonMutex->unlock();
    } else {
        LOG_WARNING("mutex not set");
    }
}

void Queue::processEvent()
{
    m_hpr->isBusy = true;
    for (;;) {
        m_hpr->eventMutex.lock();
        if (m_hpr->taskQueue.empty()) {
            m_hpr->eventMutex.unlock();
            break;
        }
        auto cb = m_hpr->taskQueue.front();
        m_hpr->taskQueue.pop();
        m_hpr->eventMutex.unlock();
        if (cb) {
            cb();
        }
    }
    for (;;) {
        m_hpr->eventMutex.lock();
        if (m_hpr->eventQueue.empty()) {
            m_hpr->eventMutex.unlock();
            break;
        }
        auto event = m_hpr->eventQueue.front();
        m_hpr->eventQueue.pop();
        m_hpr->eventMutex.unlock();
        if (event) {
            if (event->type() != Event::UNKNOWN) {
                eventChanged(event);
            }
            if (event->cb()) {
                event->cb()();
            }
        } else {
            LOG_WARNING("event is null");
        }
    }
    m_hpr->isBusy = false;
}

void Queue::addTimer(const std::shared_ptr<Timer>& timer)
{
    m_hpr->eventMutex.lock();
    timer->m_queue = this;
    m_hpr->timerList.push_back(timer);
    m_hpr->eventMutex.unlock();
}

void Queue::removeTimer(const std::shared_ptr<Timer>& timer)
{
    m_hpr->eventMutex.lock();
    timer->m_queue = nullptr;
    m_hpr->timerList.remove(timer);
    m_hpr->eventMutex.unlock();
}

void Queue::removeTimer(Timer* timer)
{
    m_hpr->eventMutex.lock();
    for (auto it = m_hpr->timerList.begin(); it != m_hpr->timerList.end(); it++) {
        if ((*it).get() == timer) {
            m_hpr->timerList.erase(it);
            break;
        }
    }
    timer->m_queue = nullptr;
    m_hpr->eventMutex.unlock();
}

void Queue::wakeUpEvent()
{
    m_hpr->eventSemaphore.reset();
}

void Queue::processNextSleepTime()
{
    std::lock_guard<std::mutex> lock(m_hpr->eventMutex);
    (void)lock;
    m_hpr->timeMethod.miniTime = -1;
    m_hpr->timeMethod.remainTime = -1;
    m_hpr->timeMethod.intervalTime = 0;
    m_hpr->timeMethod.currentTime = Utils::getNanoCurrent();
#if TIMER_USE_PRECISION
    m_hpr->timeMethod.expactCount = 0;
    for (const auto& timer : m_hpr->timerList) {
        if (!timer) {
            continue;
        }
        if (!timer->active()) {
            continue;
        }
        m_hpr->timeMethod.intervalTime = (uint64_t)timer->interval() * 1000000U;
        if (m_hpr->timeMethod.intervalTime <= 0) {
            m_hpr->timeMethod.intervalTime = 100000U;
        }
        m_hpr->timeMethod.expactCount = (m_hpr->timeMethod.currentTime - timer->m_startTimer + TIMER_ERROR_OFFSET) / m_hpr->timeMethod.intervalTime;
        if (m_hpr->timeMethod.expactCount > timer->m_loopCount) {
            if (timer->m_cb) {
                for (uint64_t offset_i = 0; offset_i < m_hpr->timeMethod.expactCount - timer->m_loopCount; offset_i++) {
                    m_hpr->taskQueue.push(timer->m_cb);
                    if (!timer->loop()) {
                        timer->stop();
                        break;
                    }
                }
            }
            if (timer->active()) {
                timer->m_loopCount = m_hpr->timeMethod.expactCount;
            }
            m_hpr->timeMethod.miniTime = 0;
        } else {
            m_hpr->timeMethod.remainTime = m_hpr->timeMethod.intervalTime - (m_hpr->timeMethod.currentTime - timer->m_startTimer) % m_hpr->timeMethod.intervalTime;
            if (m_hpr->timeMethod.remainTime < 0) {
                m_hpr->timeMethod.remainTime = 0;
            }
            if (m_hpr->timeMethod.miniTime < 0) {
                m_hpr->timeMethod.miniTime = m_hpr->timeMethod.remainTime;
            } else {
                if (m_hpr->timeMethod.remainTime < m_hpr->timeMethod.miniTime) {
                    m_hpr->timeMethod.miniTime = m_hpr->timeMethod.remainTime;
                }
            }
        }
    }
#else
    for (const auto& timer : m_hpr->timerList) {
        if (!timer) {
            continue;
        }
        if (!timer->active()) {
            continue;
        }
        m_hpr->timeMethod.intervalTime = (uint64_t)timer->interval() * 1000000U;
        if (m_hpr->timeMethod.intervalTime <= 0) {
            m_hpr->timeMethod.intervalTime = 100000U;
        }
        m_hpr->timeMethod.remainTime = timer->m_startTimer + m_hpr->timeMethod.intervalTime - m_hpr->timeMethod.currentTime - TIMER_ERROR_OFFSET;
        if (m_hpr->timeMethod.remainTime <= 0) {
            timer->m_startTimer = m_hpr->timeMethod.currentTime;
            if (timer->cb()) {
                m_hpr->taskQueue.push(timer->cb());
                if (!timer->loop()) {
                    timer->stop();
                    break;
                }
            }
            m_hpr->timeMethod.miniTime = 0;
        } else {
            if (m_hpr->timeMethod.miniTime < 0) {
                m_hpr->timeMethod.miniTime = m_hpr->timeMethod.remainTime;
            } else {
                if (m_hpr->timeMethod.remainTime < m_hpr->timeMethod.miniTime) {
                    m_hpr->timeMethod.miniTime = m_hpr->timeMethod.remainTime;
                }
            }
        }
    }
#endif
}

MIFSA_NAMESPACE_END
