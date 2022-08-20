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

#ifndef MIFSA_BASE_THREAD_H
#define MIFSA_BASE_THREAD_H

#include "mifsa/base/define.h"
#include <functional>
#include <stdint.h>
#include <string>

MIFSA_NAMESPACE_BEGIN

class MIFSA_EXPORT Thread final {
    CLASS_DISSABLE_COPY_AND_ASSIGN(Thread)
private:
    void create();

public:
    using Callback = std::function<void()>;
    Thread();
    template <typename T, typename... ARGS>
    explicit Thread(T&& cb, ARGS&&... args)
    {
        create();
        m_runCb = std::function<typename std::result_of<T(ARGS...)>::type()>(std::bind(std::forward<T>(cb), std::forward<ARGS>(args)...));
    }
    ~Thread();
    static Thread* currentThread();
    static void sleep(uint32_t s);
    static void sleepMilli(uint32_t milli_s);
    static void sleepNano(uint64_t nano_s);
    static void yield();
    static std::string currentThreadId();

public:
    bool isRunning() const;
    bool isReadyFinished() const;
    void setReadyFinished(bool readFinished);
    void setBeginCb(const Callback& cb);
    void setEndCb(const Callback& cb);
    bool start();
    template <typename T, typename... ARGS>
    bool start(T&& cb, ARGS&&... args)
    {
        std::function<typename std::result_of<T(ARGS...)>::type()> task(std::bind(std::forward<T>(cb), std::forward<ARGS>(args)...));
        m_runCb = task;
        return start();
    }
    bool stop(uint32_t milli_s = 100);

private:
    struct ThreadHelper* m_threadHelper = nullptr;
    Callback m_runCb = nullptr;
};

MIFSA_NAMESPACE_END

#endif // MIFSA_BASE_THREAD_H
