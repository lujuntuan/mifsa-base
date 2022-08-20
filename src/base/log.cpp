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

#include "mifsa/base/log.h"
#include "mifsa/base/define.h"
#include <sstream>
#ifdef MIFSA_SUPPORT_DLT
#include <dlt/dlt.h>
#endif
#include <mutex>

#define OPCOLO_RESET "\033[0m"
#define OPCOLO_UNDERLINE "\33[4m"
#define OPCOLO_BLACK "\033[30m" /* Black */
#define OPCOLO_RED "\033[31m" /* Red */
#define OPCOLO_GREEN "\033[32m" /* Green */
#define OPCOLO_YELLOW "\033[33m" /* Yellow */
#define OPCOLO_BLUE "\033[34m" /* Blue */
#define OPCOLO_MAGENTA "\033[35m" /* Magenta */
#define OPCOLO_CYAN "\033[36m" /* Cyan */
#define OPCOLO_WHITE "\033[37m" /* White */
#define OPCOLO_BACKGROUND_BLACK "\033[40;37m" /* Background Black */
#define OPCOLO_BACKGROUND_RED "\033[41;37m" /* Background Red */
#define OPCOLO_BACKGROUND_GREEN "\033[42;37m" /* Background Green */
#define OPCOLO_BACKGROUND_YELLOW "\033[43;37m" /* Background Yellow */
#define OPCOLO_BACKGROUND_BLUE "\033[44;37m" /* Background Blue */
#define OPCOLO_BACKGROUND_MAGENTA "\033[45;37m" /* Background Magenta */
#define OPCOLO_BACKGROUND_CYAN "\033[46;37m" /* Background Cyan */
#define OPCOLO_BACKGROUND_WHITE "\033[47;37m" /* Background White */
#define OPCOLO_BOLD_BLACK "\033[1m\033[30m" /* Bold Black */
#define OPCOLO_BOLD_RED "\033[1m\033[31m" /* Bold Red */
#define OPCOLO_BOLD_GREEN "\033[1m\033[32m" /* Bold Green */
#define OPCOLO_BOLD_YELLOW "\033[1m\033[33m" /* Bold Yellow */
#define OPCOLO_BOLD_BLUE "\033[1m\033[34m" /* Bold Blue */
#define OPCOLO_BOLD_MAGENTA "\033[1m\033[35m" /* Bold Magenta */
#define OPCOLO_BOLD_CYAN "\033[1m\033[36m" /* Bold Cyan */
#define OPCOLO_BOLD_WHITE "\033[1m\033[37m" /* Bold White */

std::ostringstream mifsaDebugStream_;
std::ostringstream mifsaWarningStream_;
std::ostringstream mifsaCriticalStream_;
std::ostringstream mifsaPropertyStream_;
std::mutex mifsaLogMutex_;

#ifdef MIFSA_SUPPORT_DLT
DLT_DECLARE_CONTEXT(dltCtx_);
#endif

void mifsa_print_initialize(const std::string& name)
{
#ifdef MIFSA_SUPPORT_DLT
    DLT_REGISTER_APP(name.c_str(), "Application for Logging");
    DLT_REGISTER_CONTEXT(dltCtx_, "Context", "Context for Logging");
#else
    MIFSA_UNUSED(name);
#endif
}

void mifsa_print_uninitialize()
{
#ifdef MIFSA_SUPPORT_DLT
    DLT_UNREGISTER_CONTEXT(dltCtx_);
    DLT_UNREGISTER_APP();
    dlt_free();
#endif
}

std::ostream& _mifsa_print_start(uint8_t type, uint8_t method)
{
    mifsaLogMutex_.lock();
    switch (type) {
    case MIFSA_LOG_TYPE_DEBUG:
        return mifsaDebugStream_;
    case MIFSA_LOG_TYPE_WARNING:
        if (method != MIFSA_LOG_METHOD_NO_CONSOLE) {
            std::cerr << OPCOLO_RED;
        }
        return mifsaWarningStream_;
    case MIFSA_LOG_TYPE_CRITICAL:
        if (method != MIFSA_LOG_METHOD_NO_CONSOLE) {
            std::cerr << OPCOLO_BOLD_RED;
        }
        return mifsaCriticalStream_;
    case MIFSA_LOG_TYPE_PROPERTY:
        if (method != MIFSA_LOG_METHOD_NO_CONSOLE) {
            std::cout << OPCOLO_BOLD_GREEN;
        }
        return mifsaPropertyStream_;
    default:
        return mifsaDebugStream_;
    }
}

void _mifsa_print_final(std::ostream& stream, uint8_t method)
{
    std::ostringstream* ss = (std::ostringstream*)&stream;
    const std::string& log = ss->str();
    ss->clear();
    ss->str("");
    if (ss == &mifsaDebugStream_) {
        if (method != MIFSA_LOG_METHOD_NO_CONSOLE) {
            std::cout << log;
            std::cout << std::endl;
        }
        if (method != MIFSA_LOG_METHOD_ONLY_CONSOLE) {
#ifdef MIFSA_SUPPORT_DLT
            DLT_LOG(dltCtx_, DLT_LOG_DEBUG, DLT_STRING(log.c_str()));
#endif
        }
    } else if (ss == &mifsaWarningStream_) {
        if (method != MIFSA_LOG_METHOD_NO_CONSOLE) {
            std::cerr << log;
            std::cerr << OPCOLO_RESET;
            std::cerr << std::endl;
        }
        if (method != MIFSA_LOG_METHOD_ONLY_CONSOLE) {
#ifdef MIFSA_SUPPORT_DLT
            DLT_LOG(dltCtx_, DLT_LOG_WARN, DLT_STRING(log.c_str()));
#endif
        }
    } else if (ss == &mifsaCriticalStream_) {
        if (method != MIFSA_LOG_METHOD_NO_CONSOLE) {
            std::cerr << log;
            std::cerr << OPCOLO_RESET;
            std::cerr << std::endl;
        }
        throw std::runtime_error(log);
        if (method != MIFSA_LOG_METHOD_ONLY_CONSOLE) {
#ifdef MIFSA_SUPPORT_DLT
            DLT_LOG(dltCtx_, DLT_LOG_ERROR, DLT_STRING(log.c_str()));
#endif
        }
    } else if (ss == &mifsaPropertyStream_) {
        if (method != MIFSA_LOG_METHOD_NO_CONSOLE) {
            std::cout << log;
            std::cout << OPCOLO_RESET;
            std::cout << std::endl;
        }
        if (method != MIFSA_LOG_METHOD_ONLY_CONSOLE) {
#ifdef MIFSA_SUPPORT_DLT
            DLT_LOG(dltCtx_, DLT_LOG_INFO, DLT_STRING(log.c_str()));
#endif
        }
    }
    mifsaLogMutex_.unlock();
}
