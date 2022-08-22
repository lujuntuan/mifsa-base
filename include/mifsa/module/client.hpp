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

#ifndef MIFSA_MODULE_CLIENT_H
#define MIFSA_MODULE_CLIENT_H

#include "mifsa/base/application.h"
#include "mifsa/base/queue.h"
#include "mifsa/base/semaphore.h"

MIFSA_NAMESPACE_BEGIN

class ClientInterfaceBase {
public:
    using CbConnected = std::function<void(bool connected)>;
    ClientInterfaceBase()
    {
        _cbConnected = [this](bool connected) {
            if (m_hasDetectConnect && m_cbDetectConnected) {
                m_cbDetectConnected(connected);
            }
            if (connected) {
                m_sema.reset(0);
            }
        };
    };
    virtual ~ClientInterfaceBase() = default;
    virtual std::string version() = 0;
    virtual bool connected() = 0;
    virtual bool waitForConnected(int timeout_ms = -1)
    {
        if (connected()) {
            return true;
        }
        return m_sema.acquire(timeout_ms);
    }
    virtual void detectConnect(const CbConnected& cb)
    {
        m_cbDetectConnected = cb;
        m_hasDetectConnect = true;
    }

public:
    CbConnected _cbConnected;

private:
    CbConnected m_cbDetectConnected;

private:
    std::atomic_bool m_hasDetectConnect { false };
    Semaphore m_sema;
};

template <class INTERFACE>
class ClientProxy : public Application, protected Queue {
    CLASS_DISSABLE_COPY_AND_ASSIGN(ClientProxy)

public:
    explicit ClientProxy<INTERFACE>(int argc, char** argv, const std::string& module = "")
        : Application(argc, argv, "mifsa_" + module + "_client", false)
        , Queue(0)
        , m_module(module)
    {
    }
    virtual ~ClientProxy<INTERFACE>() = default;
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
    virtual void eventChanged(const std::shared_ptr<Event>& event) override
    {
    }
    virtual bool connected()
    {
        if (!m_interface) {
            return false;
        }
        return m_interface->connected();
    }
    virtual bool waitForConnected(int timeout_ms = -1)
    {
        if (!m_interface) {
            return false;
        }
        return m_interface->connected();
    }
    virtual void detectConnect(const ClientInterfaceBase::CbConnected& cb)
    {
        if (!m_interface) {
            return;
        }
        return m_interface->detectConnect(cb);
    }

protected:
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

private:
    std::unique_ptr<INTERFACE> m_interface;
    std::string m_module;
};

MIFSA_NAMESPACE_END

#endif // MIFSA_MODULE_CLIENT_H
