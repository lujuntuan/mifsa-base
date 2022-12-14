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

#ifndef MIFSA_MODULE_SERVER_H
#define MIFSA_MODULE_SERVER_H

#include "mifsa/base/application.h"
#include "mifsa/base/queue.h"
#include <stdexcept>
#ifdef MIFSA_SUPPORT_SYSTEMD
#include <systemd/sd-daemon.h>
#endif

MIFSA_NAMESPACE_BEGIN

class ServerInterfaceBase {
public:
    ServerInterfaceBase() = default;
    virtual ~ServerInterfaceBase() = default;

private:
    virtual void onStarted() = 0;
    virtual void onStoped() = 0;

private:
    template <class INTERFACE>
    friend class ServerProxy;
    std::mutex mutex;
};

template <class INTERFACE>
class ServerProxy : public Application, public Queue {
    CLASS_DISSABLE_COPY_AND_ASSIGN(ServerProxy)

public:
    explicit ServerProxy<INTERFACE>(int argc, char** argv, const std::string& module = "", int queueId = 0)
        : Application(argc, argv, "mifsa_" + module + "_server", true)
        , Queue(queueId)
        , m_module(module)
    {
#ifdef MIFSA_SUPPORT_SYSTEMD
        sd_notify(0, "READY=1");
        auto systemdTimer = createTimer(1000, true, []() {
            sd_notify(0, "WATCHDOG=1");
        });
        systemdTimer->start();
#endif
    }
    virtual ~ServerProxy<INTERFACE>()
    {
#ifdef MIFSA_SUPPORT_SYSTEMD
        sd_notify(0, "STOPPING=1");
#endif
    }
    inline const std::string& module()
    {
        return m_module;
    }
    inline const std::unique_ptr<INTERFACE>& interface() const
    {
        if (!m_interface) {
            LOG_WARNING("instance is null");
        }
        return m_interface;
    }
    virtual void asyncExec(int flag = CHECK_SINGLETON | CHECK_TERMINATE) override
    {
        parserFlag(flag);
        asyncRun();
    }
    virtual int exec(int flag = CHECK_SINGLETON | CHECK_TERMINATE) override
    {
        parserFlag(flag);
        return run();
    }
    virtual void exit(int exitCode = 0) override
    {
        quit(exitCode);
    }
    virtual void begin() override
    {
        if (m_interface) {
            m_interface->onStarted();
        }
    }
    virtual void end() override
    {
        if (m_interface) {
            m_interface->onStoped();
        }
    }
    virtual void eventChanged(const std::shared_ptr<Event>& event) override
    {
    }

protected:
    template <class INTERFACE_ADAPTER>
    void loadInterface()
    {
        if (m_interface) {
            LOG_WARNING("instance has set");
        }
        m_interface = std::make_unique<INTERFACE_ADAPTER>();
    }
    void destroyInterface()
    {
        if (m_interface) {
            m_interface.reset();
        }
    }
    std::mutex& interfaceMutex()
    {
        if (!m_interface) {
            throw std::runtime_error("interface is null");
        }
        return m_interface->mutex;
    };

private:
    std::unique_ptr<INTERFACE> m_interface;
    std::string m_module;
};

MIFSA_NAMESPACE_END

#endif // MIFSA_MODULE_SERVER_H
