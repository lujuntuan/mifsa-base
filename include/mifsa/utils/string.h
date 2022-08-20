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

#ifndef MIFSA_UTILS_STRING_H
#define MIFSA_UTILS_STRING_H

#include "mifsa/base/define.h"
#include <string>
#include <vector>

MIFSA_NAMESPACE_BEGIN

namespace Utils {
extern MIFSA_EXPORT void stringReplace(std::string& strBase, const std::string& strSrc, const std::string& strDes) noexcept;
extern MIFSA_EXPORT void trimString(std::string& str) noexcept;
extern MIFSA_EXPORT std::vector<std::string> stringSplit(const std::string& s, const std::string& delim = ",") noexcept;
extern MIFSA_EXPORT bool stringEndWith(const std::string& str, const std::string& tail) noexcept;
extern MIFSA_EXPORT bool stringStartWith(const std::string& str, const std::string& head) noexcept;
extern MIFSA_EXPORT std::pair<std::string, std::string> getIpaddrMethod(const std::string& url) noexcept;
extern MIFSA_EXPORT std::string doubleToString(double value, int f = 2) noexcept;
template <typename... ARGS>
std::string stringSprintf(const std::string& format, ARGS... args) noexcept
{
    int length = std::snprintf(nullptr, 0, format.c_str(), args.c_str()...);
    if (length <= 0) {
        return "";
    }
    char* buf = new char[length + 1];
    std::snprintf(buf, length + 1, format.c_str(), args.c_str()...);
    std::string str(buf);
    delete[] buf;
    return str;
}
}

MIFSA_NAMESPACE_END

#endif // MIFSA_UTILS_STRING_H
