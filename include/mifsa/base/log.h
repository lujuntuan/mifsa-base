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

#ifndef MIFSA_BASE_LOG_H
#define MIFSA_BASE_LOG_H

#include "mifsa/base/define.h"
#include <iostream>

#define MIFSA_LOG_TYPE_DEBUG 0
#define MIFSA_LOG_TYPE_WARNING 1
#define MIFSA_LOG_TYPE_CRITICAL 2
#define MIFSA_LOG_TYPE_PROPERTY 3

#define MIFSA_LOG_METHOD_DEFAULT 10
#define MIFSA_LOG_METHOD_ONLY_CONSOLE 11
#define MIFSA_LOG_METHOD_NO_CONSOLE 12

extern MIFSA_EXPORT void mifsa_print_initialize(const std::string& name);
extern MIFSA_EXPORT void mifsa_print_uninitialize();
extern MIFSA_EXPORT std::ostream& _mifsa_print_start(uint8_t type, uint8_t method);
extern MIFSA_EXPORT void _mifsa_print_final(std::ostream& stream, uint8_t method);
inline void _mifsa_print_expand(std::ostream& stream, uint8_t method)
{
    _mifsa_print_final(stream, method);
}
template <typename VALUE, typename... ARGS>
inline constexpr void _mifsa_print_expand(std::ostream& stream, uint8_t method, VALUE&& value, ARGS&&... args)
{
    stream << value;
    _mifsa_print_expand(stream, method, std::forward<ARGS>(args)...);
}
template <typename T, size_t S>
inline constexpr size_t _mifsa_get_file_name(const T (&str)[S], size_t i = S - 1)
{
    return (str[i] == '/' || str[i] == '\\') ? i + 1 : (i > 0 ? _mifsa_get_file_name(str, i - 1) : 0);
}
template <typename T>
inline constexpr size_t _mifsa_get_file_name(T (&)[1])
{
    return 0;
}

#define LOG_DEBUG(...)                                                                  \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_DEBUG, MIFSA_LOG_METHOD_DEFAULT), \
        MIFSA_LOG_METHOD_DEFAULT, __VA_ARGS__)
#define LOG_WARNING(...)                                                                  \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_WARNING, MIFSA_LOG_METHOD_DEFAULT), \
        MIFSA_LOG_METHOD_DEFAULT, "[", &__FILE__[_mifsa_get_file_name(__FILE__)], "|", __LINE__, "|", __FUNCTION__, "]: ", __VA_ARGS__)
#define LOG_CRITICAL(...)                                                                                                                        \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_CRITICAL, MIFSA_LOG_METHOD_DEFAULT),                                                       \
        MIFSA_LOG_METHOD_DEFAULT, "[*CRITICAL*", &__FILE__[_mifsa_get_file_name(__FILE__)], "|", __LINE__, "|", __FUNCTION__, "]: ", __VA_ARGS__); \
    std::terminate()
#define LOG_PROPERTY(p, ...)                                                               \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_PROPERTY, MIFSA_LOG_METHOD_DEFAULT), \
        MIFSA_LOG_METHOD_DEFAULT, "[" + std::string(p) + "]: ", __VA_ARGS__)

#define LOG_DEBUG_ONLY_CONSOLE(...)                                                          \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_DEBUG, MIFSA_LOG_METHOD_ONLY_CONSOLE), \
        MIFSA_LOG_METHOD_ONLY_CONSOLE, __VA_ARGS__)
#define LOG_WARNING_ONLY_CONSOLE(...)                                                          \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_WARNING, MIFSA_LOG_METHOD_ONLY_CONSOLE), \
        MIFSA_LOG_METHOD_ONLY_CONSOLE, "[", &__FILE__[_mifsa_get_file_name(__FILE__)], "|", __LINE__, "|", __FUNCTION__, "]: ", __VA_ARGS__)
#define LOG_CRITICAL_ONLY_CONSOLE(...)                                                                                                                \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_CRITICAL, MIFSA_LOG_METHOD_ONLY_CONSOLE),                                                       \
        MIFSA_LOG_METHOD_ONLY_CONSOLE, "[*CRITICAL*", &__FILE__[_mifsa_get_file_name(__FILE__)], "|", __LINE__, "|", __FUNCTION__, "]: ", __VA_ARGS__); \
    std::terminate()
#define LOG_PROPERTY_ONLY_CONSOLE(p, ...)                                                       \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_PROPERTY, MIFSA_LOG_METHOD_ONLY_CONSOLE), \
        MIFSA_LOG_METHOD_ONLY_CONSOLE, "[" + std::string(p) + "]: ", __VA_ARGS__)

#define LOG_DEBUG_NO_CONSOLE(...)                                                          \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_DEBUG, MIFSA_LOG_METHOD_NO_CONSOLE), \
        MIFSA_LOG_METHOD_NO_CONSOLE, __VA_ARGS__)
#define LOG_WARNING_NO_CONSOLE(...)                                                          \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_WARNING, MIFSA_LOG_METHOD_NO_CONSOLE), \
        MIFSA_LOG_METHOD_NO_CONSOLE, "[", &__FILE__[_mifsa_get_file_name(__FILE__)], "|", __LINE__, "|", __FUNCTION__, "]: ", __VA_ARGS__)
#define LOG_CRITICAL_NO_CONSOLE(...)                                                                                                                \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_CRITICAL, MIFSA_LOG_METHOD_NO_CONSOLE),                                                       \
        MIFSA_LOG_METHOD_NO_CONSOLE, "[*CRITICAL*", &__FILE__[_mifsa_get_file_name(__FILE__)], "|", __LINE__, "|", __FUNCTION__, "]: ", __VA_ARGS__); \
    std::terminate()
#define LOG_PROPERTY_NO_CONSOLE(p, ...)                                                       \
    _mifsa_print_expand(_mifsa_print_start(MIFSA_LOG_TYPE_PROPERTY, MIFSA_LOG_METHOD_NO_CONSOLE), \
        MIFSA_LOG_METHOD_NO_CONSOLE, "[" + std::string(p) + "]: ", __VA_ARGS__)

#endif // MIFSA_BASE_LOG_H
