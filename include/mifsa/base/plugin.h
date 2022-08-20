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

#ifndef MIFSA_BASE_PLUGIN_H
#define MIFSA_BASE_PLUGIN_H

#include "mifsa/base/define.h"
#include <deque>
#include <memory>
#include <string>

MIFSA_NAMESPACE_BEGIN

class MIFSA_EXPORT Plugin final {
    CLASS_DISSABLE_COPY_AND_ASSIGN(Plugin)
public:
    using Handle = void*;
    explicit Plugin();
    ~Plugin();

public:
    static std::deque<std::string> defaultSearchPath();
    template <class T>
    std::shared_ptr<T> load(const std::string& libName,
        const std::deque<std::string>& searchPaths = defaultSearchPath(),
        const std::string& dirName = "",
        const std::string& functionName = "create_handle",
        uint16_t versionMajor = 0,
        uint16_t versionMinor = 0)
    {
        Handle hanlde = load(T::GetPluginId(), libName, searchPaths, dirName, functionName, versionMajor, versionMinor);
        if (!hanlde) {
            return std::shared_ptr<T>();
        }
        return std::shared_ptr<T>(static_cast<T*>(hanlde));
    }
    template <class T>
    bool unload()
    {
        return unload(T::GetPluginId());
    }
    template <class T>
    bool hasLoaded()
    {
        return hasLoaded(T::GetPluginId());
    }
    void clear();

private:
    Handle load(const std::string& pluginId,
        const std::string& libName,
        const std::deque<std::string>& searchPaths,
        const std::string& dirName,
        const std::string& functionName,
        uint16_t versionMajor,
        uint16_t versionMinor);
    bool unload(const std::string& pluginId);
    bool hasLoaded(const std::string& pluginId);

private:
    struct PluginHelper* m_pluginHelper = nullptr;
};

MIFSA_NAMESPACE_END

#define MIFSA_PLUGIN_REGISTER(PluginId) \
public:                                 \
    static std::string GetPluginId()    \
    {                                   \
        return PluginId;                \
    }                                   \
                                        \
private:

#endif // MIFSA_BASE_PLUGIN_H
