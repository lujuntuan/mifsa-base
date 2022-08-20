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

#ifndef MIFSA_UTILS_TIME_H
#define MIFSA_UTILS_TIME_H

#include "mifsa/base/define.h"
#include <ctime>
#include <string>

MIFSA_NAMESPACE_BEGIN

namespace Utils {
extern MIFSA_EXPORT std::string getCurrentTimeString(const char* format = "%Y-%m-%d %H:%M:%S") noexcept;
extern MIFSA_EXPORT uint32_t getCurrentTimeSecForString(const std::string& time_str, const char* format = "%Y-%m-%d %H:%M:%S") noexcept;
extern MIFSA_EXPORT uint32_t getCurrent() noexcept;
extern MIFSA_EXPORT uint32_t getMilliCurrent() noexcept;
extern MIFSA_EXPORT uint64_t getMicroCurrent() noexcept;
extern MIFSA_EXPORT uint64_t getNanoCurrent() noexcept;
extern MIFSA_EXPORT void sleep(uint32_t s) noexcept;
extern MIFSA_EXPORT void sleepMilli(uint32_t milli_s) noexcept;
extern MIFSA_EXPORT void sleepMicro(uint64_t micro_s) noexcept;
extern MIFSA_EXPORT void sleepNano(uint64_t nano_s) noexcept;
}

MIFSA_NAMESPACE_END

#endif // MIFSA_UTILS_TIME_H
