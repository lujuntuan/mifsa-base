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

#include "mifsa/base/elapsed.h"
#include "mifsa/utils/time.h"

MIFSA_NAMESPACE_BEGIN

void Elapsed::start(uint32_t milli_s) noexcept
{
    if (m_startTime > 0) {
        return;
    }
    start_p((uint64_t)milli_s * 1000000U);
}

void Elapsed::start_nano(uint64_t nano_s) noexcept
{
    if (m_startTime > 0) {
        return;
    }
    start_p(nano_s);
}

uint32_t Elapsed::get(uint32_t milli_s) const noexcept
{
    if (m_startTime <= 0) {
        return 0;
    }
    return (uint32_t)(get_p(milli_s) / 1000000U);
}

uint64_t Elapsed::get_nano(uint64_t nano_s) const noexcept
{
    if (m_startTime <= 0) {
        return 0;
    }
    return get_p(nano_s);
}

uint32_t Elapsed::restart(uint32_t milli_s) noexcept
{
    if (m_startTime <= 0) {
        start_p(milli_s * 1000000U);
        return 0;
    }
    uint32_t elapsedTime = (uint32_t)(get_p() / 1000000U);
    start_p(milli_s * 1000000U);
    return elapsedTime;
}

uint64_t Elapsed::restart_nano(uint64_t nano_s) noexcept
{
    if (m_startTime <= 0) {
        start_p(nano_s);
        return 0;
    }
    uint64_t elapsedTime = get_p();
    start_p(nano_s);
    return elapsedTime;
}

void Elapsed::stop() noexcept
{
    m_startTime = 0;
}

bool Elapsed::active() const noexcept
{
    return m_startTime > 0;
}

uint32_t Elapsed::current() noexcept
{
    return Utils::getMilliCurrent();
}

uint64_t Elapsed::current_nano() noexcept
{
    return Utils::getNanoCurrent();
}

void Elapsed::start_p(uint64_t nano_s) noexcept
{
    m_startTime = (nano_s == 0 ? current_nano() : nano_s);
}

uint64_t Elapsed::get_p(uint64_t nano_s) const noexcept
{
    uint64_t end = (nano_s == 0 ? current_nano() : nano_s);
    return end - m_startTime;
}

MIFSA_NAMESPACE_END
