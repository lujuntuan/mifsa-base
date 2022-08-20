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

#ifndef MIFSA_UTILS_HOST_H
#define MIFSA_UTILS_HOST_H

#include "mifsa/base/define.h"
#include <string>
#include <vector>

MIFSA_NAMESPACE_BEGIN

namespace Utils {
extern MIFSA_EXPORT std::string getExePath() noexcept;
extern MIFSA_EXPORT std::string getExeDir(const std::string& path = "") noexcept;
extern MIFSA_EXPORT std::string getExeName(const std::string& path = "") noexcept;
extern MIFSA_EXPORT std::string getIpAddress(const std::string& ethName = "") noexcept;
extern MIFSA_EXPORT std::string getHostName() noexcept;
extern MIFSA_EXPORT std::string getEnvironment(const std::string& key, const std::string& defaultValue = "") noexcept;
extern MIFSA_EXPORT bool setEnvironment(const std::string& key, const std::string& value, bool force = true) noexcept;
}

MIFSA_NAMESPACE_END

#endif // MIFSA_UTILS_HOST_H
