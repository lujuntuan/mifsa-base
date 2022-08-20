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

#ifndef MIFSA_BASE_SEMAPHORE_H
#define MIFSA_BASE_SEMAPHORE_H

#include "mifsa/base/define.h"
#include <atomic>
#include <condition_variable>
#include <mutex>

MIFSA_NAMESPACE_BEGIN

class MIFSA_EXPORT Semaphore final {
public:
    explicit Semaphore(uint32_t count = 0) noexcept;
    uint32_t count() const noexcept;
    bool acquire(int32_t milli_s = -1) noexcept;
    bool acquire_nano(int64_t nano_s = -1) noexcept;
    bool release() noexcept;
    bool reset(uint32_t count = 0) noexcept;

private:
    std::atomic<int32_t> m_count { 0 };
    std::mutex m_mutex;
    std::condition_variable m_condition;
};

MIFSA_NAMESPACE_END

#endif // MIFSA_BASE_SEMAPHORE_H
