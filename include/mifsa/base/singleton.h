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

#ifndef MIFSA_BASE_SINGLETON_H
#define MIFSA_BASE_SINGLETON_H

#include "mifsa/base/define.h"
#include "mifsa/base/log.h"
#include <memory>

MIFSA_NAMESPACE_BEGIN
template <typename T>
class SingletonProxy {
public:
    static T* getInstance(bool check = true)
    {
        if (check && !m_instance) {
            LOG_CRITICAL("instance is null");
        }
        return m_instance;
    }
    static void setInstance(T* instance)
    {
        if (instance && m_instance) {
            LOG_CRITICAL("instance has set");
            return;
        }
        m_instance = instance;
    }
    static void setInstanceForce(T* instance)
    {
        m_instance = instance;
    }

private:
    static T* m_instance;
};

template <typename T>
T* SingletonProxy<T>::m_instance = nullptr;

MIFSA_NAMESPACE_END

#endif // MIFSA_BASE_SINGLETON_H
