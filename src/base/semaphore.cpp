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

#include "mifsa/base/semaphore.h"
#include <chrono>

MIFSA_NAMESPACE_BEGIN

Semaphore::Semaphore(uint32_t count) noexcept
    : m_count(count)
{
}

uint32_t Semaphore::count() const noexcept
{
    return m_count;
}

bool Semaphore::acquire(int32_t milli_s) noexcept
{
    return acquire_nano((uint64_t)milli_s * 1000000U);
}

bool Semaphore::acquire_nano(int64_t nano_s) noexcept
{
    if (m_count <= 0 && nano_s == 0) {
        return false;
    }
    std::unique_lock<std::mutex> lock(m_mutex);
    m_count--;
    if (m_count < 0) {
        if (nano_s < 0) {
            m_condition.wait(lock);
        } else {
            if (m_condition.wait_for(lock, std::chrono::nanoseconds(nano_s)) == std::cv_status::timeout) {
                m_count++;
                return false;
            }
        }
    } else {
        return false;
    }
    return true;
}

bool Semaphore::release() noexcept
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_count < 0) {
        m_condition.notify_one();
        m_count++;
        return true;
    }
    m_count++;
    return false;
}

bool Semaphore::reset(uint32_t count) noexcept
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_count < 0) {
        m_condition.notify_all();
        m_count = count;
        return true;
    }
    m_count = count;
    return false;
}

MIFSA_NAMESPACE_END
