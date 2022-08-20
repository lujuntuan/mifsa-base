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
#include <mifsa/base/semaphore.h>
#include <mifsa/base/thread.h>

using namespace Mifsa;

int main(int argc, char* argv[])
{
    Semaphore sema(5);
    Thread thread1;
    Thread thread2;
    Thread thread3;
    Thread thread4;
    Thread thread5;
    thread1.start([&]() {
        while (!thread1.isReadyFinished()) {
            sema.acquire();
            LOG_DEBUG("thread1 acquire success");
        }
    });
    thread2.start([&]() {
        while (!thread2.isReadyFinished()) {
            sema.acquire();
            LOG_DEBUG("thread2 acquire success");
        }
    });
    thread3.start([&]() {
        while (!thread3.isReadyFinished()) {
            sema.acquire();
            LOG_DEBUG("thread3 acquire success");
        }
    });
    thread4.start([&]() {
        while (!thread4.isReadyFinished()) {
            sema.acquire();
            LOG_DEBUG("thread4 acquire success");
        }
    });
    thread5.start([&]() {
        while (!thread5.isReadyFinished()) {
            sema.acquire();
            LOG_DEBUG("thread5 acquire success");
        }
    });

    sema.release();
    Thread::sleep(1);
    sema.release();
    Thread::sleep(1);
    sema.release();
    Thread::sleep(1);

    thread1.setReadyFinished(true);
    thread2.setReadyFinished(true);
    thread3.setReadyFinished(true);
    thread4.setReadyFinished(true);
    thread5.setReadyFinished(true);
    sema.reset(5);
    return 0;
}
