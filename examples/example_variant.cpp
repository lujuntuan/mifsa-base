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

#include <thread>
#include <mifsa/base/log.h>
#include <mifsa/base/thread.h>
#include <mifsa/base/variant.h>

using namespace Mifsa;

int main(int argc, char* argv[])
{
    VariantMap data = {
        { "name", "zhangsan" },
        { "arg", 18 },
        { "phone", "1234567" },
        { "temperature", 37.8f },
        { "address", VariantList { "home", "school" } },
        { "other", VariantList { 1.0, 2.0 } },
    };
    data.saveJson("test_variant.json");
    Thread::sleep(1);
    Variant readData = Variant::readJson("test_variant.json");
    if (readData == data) {
        LOG_DEBUG("values are equal");
    } else {
        LOG_DEBUG("values are not equal");
    }
    return 0;
}
