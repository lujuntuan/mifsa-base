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

#ifndef MIFSA_MODULE_INTERFACE_H
#define MIFSA_MODULE_INTERFACE_H

#include "mifsa/base/log.h"
#include "mifsa/base/semaphore.h"
#include <atomic>
#include <functional>

MIFSA_NAMESPACE_BEGIN

using CbConnected = std::function<void(bool connected)>;

class InterfaceBase {
public:
    InterfaceBase()
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
    virtual ~InterfaceBase() = default;
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

protected:
    CbConnected _cbConnected;

private:
    CbConnected m_cbDetectConnected;

private:
    std::atomic_bool m_hasDetectConnect { false };
    Semaphore m_sema;
};

MIFSA_NAMESPACE_END

#endif // MIFSA_MODULE_INTERFACE_H
