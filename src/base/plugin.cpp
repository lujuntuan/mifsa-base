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

#include "mifsa/base/plugin.h"
#include "importlib/dylib.hpp"
#include "mifsa/base/log.h"
#include "mifsa/utils/dir.h"
#include <map>

#define m_hpr m_pluginHelper

MIFSA_NAMESPACE_BEGIN

struct PluginHelper {
    std::map<std::string, std::unique_ptr<dylib>> pluginMap;
};

Plugin::Plugin()
{
    MIFSA_HELPER_CREATE(m_hpr);
}

Plugin::~Plugin()
{
    clear();
    MIFSA_HELPER_DESTROY(m_hpr);
}

std::deque<std::string> Plugin::defaultSearchPath()
{
    return {
        ".",
        "/lib64",
        "/lib",
        "../lib64",
        "../lib",
        "./lib64",
        "./lib",
    };
}

void Plugin::clear()
{
    std::map<std::string, std::unique_ptr<dylib>> tmp;
    m_hpr->pluginMap.clear();
    m_hpr->pluginMap.swap(tmp);
}

Plugin::Handle Plugin::load(const std::string& pluginId,
    const std::string& libName,
    const std::deque<std::string>& searchPaths,
    const std::string& dirName,
    const std::string& functionName,
    uint16_t versionMajor,
    uint16_t versionMinor)
{
    if (pluginId.empty()) {
        LOG_WARNING("plugin id is empty");
        return nullptr;
    }
    if (hasLoaded(pluginId)) {
        LOG_WARNING("plugin has loaded");
        return nullptr;
    }
    std::string pluginName = libName + dylib::filename_components::suffix;
    // std::string pluginName = dylib::filename_components::prefix + libName + dylib::filename_components::suffix;
    //  Remove, cmake project has removed the prefix.
    std::string pluginPath;
    {
        std::string checkPath;
        for (const auto& path : searchPaths) {
            if (!dirName.empty()) {
                checkPath = path + "/" + dirName + "/" + pluginName;
                if (Utils::exists(checkPath)) {
                    pluginPath = std::move(checkPath);
                    break;
                }
            }
            checkPath = path + pluginName;
            if (Utils::exists(checkPath)) {
                pluginPath = std::move(checkPath);
                break;
            }
        }
    }
    Handle handle = nullptr;
    if (pluginPath.empty()) {
        LOG_WARNING("can not find plugin", "(", libName, ")");
        return handle;
    }
    LOG_DEBUG("loading plugin: ", pluginPath);
    try {
        std::unique_ptr<dylib> loader = std::make_unique<dylib>(std::move(pluginPath), false);
        auto createFunction = loader->get_function<Handle(const char*, uint16_t, uint16_t)>(functionName);
        if (!createFunction) {
            LOG_WARNING("can not find symbol function", "(", libName, ")");
            return handle;
        }
        handle = createFunction(pluginId.c_str(), versionMajor, versionMinor);
        if (!handle) {
            LOG_WARNING("failed to load plugin", "(", libName, ")");
            return handle;
        }
        LOG_DEBUG("plugin loaded successfully");
        m_hpr->pluginMap.emplace(pluginId, loader.release());
        return handle;
    } catch (const dylib::exception& e) {
        LOG_WARNING(e.what());
        return handle;
    }
}

bool Plugin::unload(const std::string& pluginId)
{
    if (pluginId.empty()) {
        LOG_WARNING("plugin id is empty");
        return false;
    }
    if (!hasLoaded(pluginId)) {
        LOG_WARNING("plugin not loaded");
        return false;
    }
    m_hpr->pluginMap.erase(pluginId);
    return true;
}

bool Plugin::hasLoaded(const std::string& pluginId)
{
    if (m_hpr->pluginMap.find(pluginId) != m_hpr->pluginMap.end()) {
        return true;
    }
    return false;
}

MIFSA_NAMESPACE_END
