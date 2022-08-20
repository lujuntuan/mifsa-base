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
#include <mifsa/base/thread.h>

using namespace Mifsa;

int main(int argc, char* argv[])
{
    LOG_DEBUG("main thread id:", Thread::currentThreadId());
    Thread thread;
    thread.start([&]() {
        LOG_DEBUG("thread start");
        LOG_DEBUG("in thread id:", Thread::currentThreadId());
        while (!thread.isReadyFinished()) {
            LOG_DEBUG("..");
            Thread::sleep(1);
        }
        LOG_DEBUG("thread end");
    });
    Thread::sleep(10);
    thread.stop();
    return 0;
}
