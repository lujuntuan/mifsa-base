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

#include <mifsa/base/log.h>
#include <mifsa/base/pool.h>
#include <mifsa/base/thread.h>

using namespace Mifsa;

int main(int argc, char* argv[])
{
    Pool pool(8);
    pool.enqueue([&]() {
        LOG_DEBUG(Thread::currentThreadId());
    });
    pool.enqueue([&]() {
        LOG_DEBUG(Thread::currentThreadId());
    });
    pool.enqueue([&]() {
        LOG_DEBUG(Thread::currentThreadId());
    });
    pool.enqueue([&]() {
        LOG_DEBUG(Thread::currentThreadId());
    });
    pool.enqueue([&]() {
        LOG_DEBUG(Thread::currentThreadId());
    });
    pool.enqueue([&]() {
        LOG_DEBUG(Thread::currentThreadId());
    });
    pool.enqueue([&]() {
        LOG_DEBUG(Thread::currentThreadId());
    });
    pool.enqueue([&]() {
        LOG_DEBUG(Thread::currentThreadId());
    });
    pool.enqueue([&]() {
        LOG_DEBUG(Thread::currentThreadId());
    });
    Thread::sleep(10);
    pool.shutdown();
}
