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

#include "mifsa/base/thread.h"
#include "mifsa/base/log.h"
#include "mifsa/utils/time.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <sstream>
#include <thread>
#include <unordered_map>

#define m_hpr m_threadHelper

MIFSA_NAMESPACE_BEGIN

static std::mutex& getThreadMapLock()
{
    static std::mutex mutex;
    return mutex;
}

static std::unordered_map<std::thread::id, Thread*>& getThreadMap()
{
    static std::unordered_map<std::thread::id, Thread*> threadMap;
    return threadMap;
}

struct ThreadHelper {
    std::unique_ptr<std::thread> thread;
    std::atomic_bool isRunning { false };
    std::atomic_bool isReadyFinished { true };
    Thread::Callback beginCb = nullptr;
    Thread::Callback endCb = nullptr;
    std::mutex mutex;
    std::condition_variable_any condition;
};

void Thread::create()
{
    MIFSA_HELPER_CREATE(m_hpr);
}

Thread::Thread()
{
    create();
}

Thread::~Thread()
{
    if (m_hpr->thread) {
        stop();
    }
    MIFSA_HELPER_DESTROY(m_hpr);
}

Thread* Thread::currentThread()
{
    getThreadMapLock().lock();
    const auto& thread = getThreadMap().find(std::this_thread::get_id())->second;
    getThreadMapLock().unlock();
    return thread;
}

void Thread::sleep(uint32_t s)
{
    Utils::sleep(s);
}

void Thread::sleepMilli(uint32_t milli_s)
{
    Utils::sleepMilli(milli_s);
}

void Thread::sleepNano(uint64_t nano_s)
{
    Utils::sleepNano(nano_s);
}

void Thread::yield()
{
    std::this_thread::yield();
}

std::string Thread::currentThreadId()
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}

bool Thread::isRunning() const
{
    return m_hpr->isRunning;
}

bool Thread::isReadyFinished() const
{
    return m_hpr->isReadyFinished;
}

void Thread::setReadyFinished(bool readFinished)
{
    m_hpr->isReadyFinished = readFinished;
}

void Thread::setBeginCb(const Callback& cb)
{
    m_hpr->beginCb = cb;
}

void Thread::setEndCb(const Callback& cb)
{
    m_hpr->endCb = cb;
}

bool Thread::start()
{
    if (!m_runCb) {
        return false;
    }
    if (m_hpr->isRunning) {
        return false;
    }
    if (m_hpr->thread) {
        stop();
    }
    m_hpr->isReadyFinished = false;
    m_hpr->isRunning = true;
    try {
        m_hpr->thread = std::make_unique<std::thread>([this]() {
            getThreadMapLock().lock();
            getThreadMap().emplace(std::this_thread::get_id(), this);
            getThreadMapLock().unlock();
            if (m_hpr->beginCb) {
                m_hpr->beginCb();
            }
            m_runCb();
            if (m_hpr->endCb) {
                m_hpr->endCb();
            }
            m_hpr->isRunning = false;
            std::lock_guard<std::mutex> lock(m_hpr->mutex);
            m_hpr->condition.notify_one();
            getThreadMapLock().lock();
            getThreadMap().erase(std::this_thread::get_id());
            getThreadMapLock().unlock();
        });
        return true;
    } catch (...) {
        LOG_WARNING("create thread error");
        return false;
    }
}

bool Thread::stop(uint32_t milli_s)
{
    m_hpr->isReadyFinished = true;
    if (m_hpr->isRunning) {
        if (!m_hpr->thread) {
            return false;
        }
        if (!m_hpr->thread->joinable()) {
            return true;
        }
    } else {
        if (m_hpr->thread) {
            if (m_hpr->thread->joinable()) {
                m_hpr->thread->join();
            }
        }
        return true;
    }
    std::unique_lock<std::mutex> lock(m_hpr->mutex);
    if (milli_s > 0) {
        m_hpr->condition.wait_for(m_hpr->mutex, std::chrono::milliseconds(std::move(milli_s)));
    } else {
        m_hpr->condition.wait(m_hpr->mutex);
    }
    if (m_hpr->isRunning) {
        LOG_WARNING("thread is forced to end");
        try {
            m_hpr->thread->detach();
            auto nativeHandle = m_hpr->thread->native_handle();
            MIFSA_UNUSED(nativeHandle);
#ifdef HAS_PTHREAD
            pthread_cancel(nativeHandle);
#else
#ifdef WIN32
            // kill thread
#endif
#endif
        } catch (...) {
            LOG_WARNING("destroy thread error");
            m_hpr->isRunning = false;
            return false;
        }
    } else {
        m_hpr->thread->join();
    }
    m_hpr->isRunning = false;
    return true;
}

MIFSA_NAMESPACE_END
