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

#include "mifsa/base/pool.h"
#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#define m_hpr m_poolHelper

MIFSA_NAMESPACE_BEGIN

struct PoolHelper {
    std::vector<std::thread> threads;
    std::list<Pool::Callback> tasks;
    std::atomic_bool exitFlag { false };
    std::condition_variable condition;
    std::mutex mutex;
};

Pool::Pool(uint32_t thread_count)
{
    MIFSA_HELPER_CREATE(m_hpr);
    for (uint32_t i = 0; i < thread_count; i++) {
        m_hpr->threads.emplace_back(std::thread([this]() {
            for(;;) {
                Callback task;
                {
                    std::unique_lock<std::mutex> lock(m_hpr->mutex);
                    m_hpr->condition.wait(lock, [&] { return !m_hpr->tasks.empty() || m_hpr->exitFlag; });
                    if (m_hpr->exitFlag && m_hpr->tasks.empty()) {
                        break;
                    }
                    task = m_hpr->tasks.front();
                    m_hpr->tasks.pop_front();
                }
                if (task) {
                    task();
                }
            }
        }));
    }
}

Pool::~Pool()
{
    MIFSA_HELPER_DESTROY(m_hpr);
}

void Pool::enqueue(const Callback& task)
{
    std::unique_lock<std::mutex> lock(m_hpr->mutex);
    m_hpr->tasks.push_back(std::move(task));
    m_hpr->condition.notify_one();
}

void Pool::shutdown()
{
    m_hpr->exitFlag = true;
    m_hpr->condition.notify_all();
    for (auto& thread : m_hpr->threads) {
        thread.join();
    }
}

MIFSA_NAMESPACE_END
