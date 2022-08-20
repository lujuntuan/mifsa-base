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

#ifndef MIFSA_MODULE_PROVIDER_H
#define MIFSA_MODULE_PROVIDER_H

#include "mifsa/base/log.h"
#include <functional>

MIFSA_NAMESPACE_BEGIN

class ProviderBase {
public:
    ProviderBase() = default;
    virtual ~ProviderBase() = default;
};

MIFSA_NAMESPACE_END

#endif // MIFSA_MODULE_PROVIDER_H
