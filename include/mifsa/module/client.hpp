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

template <class INTERFACE>
class ClientProxy : public Application, public Queue {
    CLASS_DISSABLE_COPY_AND_ASSIGN(ClientProxy)

public:
    explicit ClientProxy<INTERFACE>(int argc, char** argv, const std::string& module = "")
        : Application(argc, argv, "mifsa_" + module + "_client", false)
        , Queue(0)
        , m_module(module)
    {
    }
    virtual ~ClientProxy<INTERFACE>() = default;
    inline const std::unique_ptr<INTERFACE>& interface() const
    {
        if (!m_interface) {
            LOG_WARNING("instance is null");
        }
        return m_interface;
    }
    inline const std::string& module()
    {
        return m_module;
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
        this->destroyInterface();
        quit(exitCode);
    }
    virtual void eventChanged(const std::shared_ptr<Event>& event) override
    {
    }

protected:
    template <class TARGET>
    void loadInterface()
    {
        if (m_interface) {
            LOG_WARNING("instance has set");
        }
        m_interface = std::make_unique<TARGET>();
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
