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

#ifndef MIFSA_BASE_ELAPSED_H
#define MIFSA_BASE_ELAPSED_H

#include "mifsa/base/define.h"
#include <atomic>
#include <stdint.h>

MIFSA_NAMESPACE_BEGIN

class MIFSA_EXPORT Elapsed final {
public:
    Elapsed() = default;
    void start(uint32_t milli_s = 0) noexcept;
    void start_nano(uint64_t nano_s = 0) noexcept;
    uint32_t get(uint32_t milli_s = 0) const noexcept;
    uint64_t get_nano(uint64_t nano_s = 0) const noexcept;
    uint32_t restart(uint32_t milli_s = 0) noexcept;
    uint64_t restart_nano(uint64_t nano_s = 0) noexcept;
    void stop() noexcept;
    bool active() const noexcept;
    static uint32_t current() noexcept;
    static uint64_t current_nano() noexcept;

private:
    void start_p(uint64_t nano_s = 0) noexcept;
    uint64_t get_p(uint64_t nano_s = 0) const noexcept;

private:
    std::atomic<uint64_t> m_startTime { 0 };
};

MIFSA_NAMESPACE_END

#endif // MIFSA_BASE_ELAPSED_H
