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

#include "mifsa/base/timer.h"
#include "mifsa/base/log.h"
#include "mifsa/base/queue.h"
#include "mifsa/utils/time.h"

MIFSA_NAMESPACE_BEGIN

Timer::Timer(const TimerPrivate&, uint32_t interval_milli_s, bool loop, const Callback& cb) noexcept
    : m_interval(interval_milli_s)
    , m_loop(loop)
    , m_cb(cb)
{
}

Timer::~Timer() noexcept
{
    m_cb = nullptr;
    stop();
    if (m_queue) {
        m_queue->removeTimer(this);
    }
}

std::shared_ptr<Timer> Timer::create(Queue* queue, uint32_t interval_milli_s, bool loop, const Callback& cb) noexcept
{
    if (!queue) {
        LOG_CRITICAL("queue is null");
    }
    const std::shared_ptr<Timer>& timer = std::make_shared<Timer>(TimerPrivate { 0 }, interval_milli_s, loop, cb);
    queue->addTimer(timer);
    return timer;
}
void Timer::once(Queue* queue, uint32_t interval_milli_s, const Callback& cb) noexcept
{
    if (!queue) {
        LOG_CRITICAL("queue is null");
    }
    queue->onceTimer(interval_milli_s, cb);
}

void Timer::start(const Callback& cb) noexcept
{
    if (cb) {
        m_cb = cb;
    }
    m_startTimer = Utils::getNanoCurrent();
#if TIMER_USE_PRECISION
    m_loopCount = 0;
#endif
    if (m_queue) {
        m_queue->wakeUpQueue();
    }
}

void Timer::stop() noexcept
{
    m_startTimer = 0;
#if TIMER_USE_PRECISION
    m_loopCount = 0;
#endif
}

bool Timer::active() const noexcept
{
    return m_startTimer > 0;
}

uint32_t Timer::interval() const noexcept
{
    return m_interval;
}

bool Timer::loop() const noexcept
{
    return m_loop;
}

void Timer::setInterval(uint32_t interval_milli_s) noexcept
{
    m_interval = interval_milli_s;
    if (active() && m_queue) {
        m_queue->wakeUpQueue();
    }
}

void Timer::setLoop(bool loop) noexcept
{
    m_loop = loop;
}

MIFSA_NAMESPACE_END
