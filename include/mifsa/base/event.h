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

#ifndef MIFSA_BASE_EVENT_H
#define MIFSA_BASE_EVENT_H

#include "mifsa/base/define.h"
#include <functional>

MIFSA_NAMESPACE_BEGIN

class Event {
public:
    using Callback = std::function<void()>;
    enum Type {
        UNKNOWN = -1
    };

protected:
    explicit Event(int queueId, int type = UNKNOWN, const Callback& cb = nullptr) noexcept
        : m_queueId(queueId)
        , m_type(type)
        , m_cb(cb)
    {
    }
    virtual ~Event() noexcept
    {
    }

public:
    inline bool valid() const noexcept
    {
        return m_type >= 0;
    }
    inline int queueId() const noexcept
    {
        return m_queueId;
    }
    inline int type() const noexcept
    {
        return m_type;
    }
    inline Callback cb() const noexcept
    {
        return m_cb;
    }
    inline int isAccepted() const noexcept
    {
        return m_accepted;
    }
    inline void setCallback(const Callback& cb) noexcept
    {
        m_cb = cb;
    }
    inline void setAccept(bool accepted) noexcept
    {
        m_accepted = accepted;
    }

private:
    friend class Queue;
    int m_queueId = -1;
    int m_type = UNKNOWN;
    Callback m_cb = nullptr;
    bool m_accepted = false;
};

MIFSA_NAMESPACE_END

#endif // MIFSA_BASE_EVENT_H
