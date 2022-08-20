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

#ifndef MIFSA_BASE_APPLICATION_H
#define MIFSA_BASE_APPLICATION_H

#include "mifsa/base/config.h"
#include "mifsa/base/define.h"
#include "mifsa/base/log.h"
#include "mifsa/base/variant.h"
#include <tuple>
#include <vector>

MIFSA_NAMESPACE_BEGIN

class MIFSA_EXPORT Application {
    CLASS_DISSABLE_COPY_AND_ASSIGN(Application)
public:
    struct Arg {
        std::string shortName;
        std::string longName;
        std::string description;
        Variant defaultValue;
        explicit Arg(const std::string& _shortName, const std::string& _longName, const std::string& _description, const Variant& _defaultValue = nullptr)
            : shortName(_shortName)
            , longName(_longName)
            , description(_description)
            , defaultValue(_defaultValue)
        {
        }
    };
    enum ApplicationFlag {
        CHECK_NONE = (1 << 1),
        CHECK_SINGLETON = (1 << 2),
        CHECK_TERMINATE = (1 << 3),
    };
    explicit Application(int argc, char** argv, const std::string& typeName = "", bool hasCfg = false);
    virtual ~Application();
    int argc() const;
    char** argv() const;
    const std::string& exePath() const;
    const std::string& exeDir() const;
    const std::string& exeName() const;
    const std::string& typeName() const;
    const VariantMap& config() const;
    Variant getArgValue(const Arg& arg) const;
    virtual void asyncExec(int flag = CHECK_SINGLETON | CHECK_TERMINATE);
    virtual int exec(int flag = CHECK_SINGLETON | CHECK_TERMINATE) = 0;
    virtual void exit(int exitCode = 0) = 0;

protected:
    void parserFlag(int flag);
    void parserArgs(const std::vector<Arg>& args);
    VariantMap readConfig(const std::string& fileName);

private:
    struct ApplicationHelper* m_applicationHelper = nullptr;
};

MIFSA_NAMESPACE_END

#endif // MIFSA_BASE_APPLICATION_H
