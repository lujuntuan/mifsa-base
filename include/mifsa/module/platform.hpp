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

#ifndef MIFSA_MODULE_PLATFORM_H
#define MIFSA_MODULE_PLATFORM_H

#include "mifsa/base/define.h"
#include "mifsa/base/log.h"
#include "mifsa/base/plugin.h"
#include <mifsa/utils/host.h>
#include <stdlib.h>

MIFSA_NAMESPACE_BEGIN

#define MIFSA_PLATFORM_FUNCTION_NAME create_mifsa_handle
#define MIFSA_PLATFORM_VERSION_MAJOR 1
#define MIFSA_PLATFORM_VERSION_MINOR 0

#define MIFSA_PLATFORM_ENV_NAME "MIFSA_PLATFORM_PATH"
#define MIFSA_PLATFORM_DIR_NAME "mifsa-plugins"
#define MIFSA_PLATFORM_PREFIX_NAME "mifsa_"
#define MIFSA_PLATFORM_MID_NAME "_platform_"

class PlatformInterfaceBase {
public:
    PlatformInterfaceBase() = default;
    virtual ~PlatformInterfaceBase() = default;
    template <class IMPLEMENTATION, int RV_MAJOR = MIFSA_PLATFORM_VERSION_MAJOR, int RV_MINOR = MIFSA_PLATFORM_VERSION_MINOR>
    static Plugin::Handle createPlatform(const char* pluginId, uint16_t versionMajor, uint16_t versionMinor)
    {
        const std::string& RealPluginId = IMPLEMENTATION::GetPluginId();
        std::cout << "platform id: " << RealPluginId << std::endl;
        std::cout << "platform version: " << RV_MAJOR << "." << RV_MINOR << std::endl;
        if (RealPluginId.empty() || RealPluginId != pluginId) {
            std::cerr << "compare platform ID error: " << pluginId << std::endl;
            return nullptr;
        }
        if (RV_MAJOR != versionMajor || RV_MINOR < versionMinor) {
            std::cerr << "compare platform version error: " << versionMajor << "." << versionMinor << std::endl;
            return nullptr;
        }
        return new IMPLEMENTATION;
    }
};

template <class INTERFACE>
class PlatformProxy {
public:
    PlatformProxy<INTERFACE>() = default;
    ~PlatformProxy<INTERFACE>()
    {
        m_platform = nullptr;
        m_plugin.clear();
    }
    inline const std::shared_ptr<INTERFACE>& platform(bool showWarn = true) const
    {
        if (showWarn && !m_platform) {
            LOG_WARNING("instance is null");
        }
        return m_platform;
    }
    void loadPlatform(const std::string& libName, const std::string& platformName = Utils::getEnvironment("MIFSA_PLATFORM", "unix"), uint16_t versionMajor = MIFSA_PLATFORM_VERSION_MAJOR, uint16_t versionMinor = MIFSA_PLATFORM_VERSION_MINOR)
    {
        if (m_platform) {
            LOG_WARNING("instance has set");
        }
        auto seartPath = m_plugin.defaultSearchPath();
        const char* path = getenv(MIFSA_PLATFORM_ENV_NAME);
        if (path) {
            seartPath.emplace_front(path);
        }
        m_platform = m_plugin.load<INTERFACE>(
            MIFSA_PLATFORM_PREFIX_NAME + libName + MIFSA_PLATFORM_MID_NAME + platformName,
            std::move(seartPath),
            MIFSA_PLATFORM_DIR_NAME,
            MIFSA_GETNAME(MIFSA_PLATFORM_FUNCTION_NAME),
            versionMajor,
            versionMinor);
        if (!m_platform) {
            std::exit(-1);
        }
    }

private:
    Plugin m_plugin;
    std::shared_ptr<INTERFACE> m_platform;
};

#define MIFSA_CREATE_PLATFORM(...)                                                                                               \
    C_INTERFACE_BEGIN                                                                                                            \
    MIFSA_EXPORT Plugin::Handle MIFSA_PLATFORM_FUNCTION_NAME(const char* pluginId, uint16_t versionMajor, uint16_t versionMinor) \
    {                                                                                                                            \
        return PlatformInterfaceBase::createPlatform<__VA_ARGS__>(pluginId, versionMajor, versionMinor);                         \
    }                                                                                                                            \
    C_INTERFACE_END

MIFSA_NAMESPACE_END

#endif // MIFSA_MODULE_PLATFORM_H
