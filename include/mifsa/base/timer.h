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

#ifndef MIFSA_BASE_TIMER_H
#define MIFSA_BASE_TIMER_H

#include "mifsa/base/define.h"
#include <atomic>
#include <functional>
#include <memory>
#include <stdint.h>

MIFSA_NAMESPACE_BEGIN

#define TIMER_USE_PRECISION 1
#define TIMER_ERROR_OFFSET 1000000U

class Queue;

class MIFSA_EXPORT Timer final {
    CLASS_DISSABLE_COPY_AND_ASSIGN(Timer)

private:
    struct TimerPrivate {
        explicit TimerPrivate(int) { }
    };

public:
    using Callback = std::function<void()>;
    explicit Timer(const TimerPrivate&, uint32_t interval_milli_s, bool loop = false, const Callback& cb = nullptr) noexcept; // 0milli_s==100000ns
    ~Timer() noexcept;

public:
    static std::shared_ptr<Timer> create(Queue* queue, uint32_t interval_milli_s, bool loop = false, const Callback& cb = nullptr) noexcept;
    static void once(Queue* queue, uint32_t interval_milli_s, const Callback& cb = nullptr) noexcept;
    void start(const Callback& cb = nullptr) noexcept;
    void stop() noexcept;
    bool active() const noexcept;
    uint32_t interval() const noexcept;
    bool loop() const noexcept;
    void setInterval(uint32_t interval_milli_s) noexcept;
    void setLoop(bool loop) noexcept;

private:
    friend Queue;
    std::atomic<uint32_t> m_interval { 0 };
    std::atomic<uint64_t> m_startTimer { 0 };
#if TIMER_USE_PRECISION
    std::atomic<uint64_t> m_loopCount = { 0 };
#endif
    std::atomic_bool m_loop { false };
    Callback m_cb = nullptr;
    Queue* m_queue = nullptr;
};

MIFSA_NAMESPACE_END

#endif // MIFSA_BASE_TIMER_H
