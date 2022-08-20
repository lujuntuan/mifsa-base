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

#include "mifsa/base/application.h"
#include "importlib/popl.hpp"
#include "mifsa/utils/dir.h"
#include "mifsa/utils/host.h"
#include "mifsa/utils/system.h"

#define m_hpr m_applicationHelper

MIFSA_NAMESPACE_BEGIN

struct ApplicationHelper {
    int argc = 0;
    char** argv = nullptr;
    popl::OptionParser argParser { "Allowed options" };
    std::string typeName;
    std::string exePath;
    std::string exeDir;
    std::string exeName;
    VariantMap config;
};

Application::Application(int argc, char** argv, const std::string& typeName, bool hasCfg)
{
    MIFSA_HELPER_CREATE(m_hpr);
    m_hpr->argc = argc;
    m_hpr->argv = argv;
    m_hpr->exePath = Utils::getExePath();
    m_hpr->exeDir = Utils::getExeDir(m_hpr->exePath);
    m_hpr->exeName = Utils::getExeName(m_hpr->exePath);
    m_hpr->typeName = typeName;
    Utils::setCurrentPath(m_hpr->exeDir);
    if (!m_hpr->typeName.empty() && hasCfg) {
        m_hpr->config = readConfig(m_hpr->typeName + ".json");
    }
    mifsa_print_initialize(typeName);
}

Application::~Application()
{
    MIFSA_HELPER_DESTROY(m_hpr);
    mifsa_print_uninitialize();
}

void Application::asyncExec(int flag)
{
    MIFSA_UNUSED(flag);
    LOG_WARNING("not support");
}

int Application::argc() const
{
    return m_hpr->argc;
}

char** Application::argv() const
{
    return m_hpr->argv;
}

const std::string& Application::exePath() const
{
    return m_hpr->exePath;
}

const std::string& Application::exeDir() const
{
    return m_hpr->exeDir;
}

const std::string& Application::exeName() const
{
    return m_hpr->exeName;
}

const std::string& Application::typeName() const
{
    return m_hpr->typeName;
}

const VariantMap& Application::config() const
{
    return m_hpr->config;
}

Variant Application::getArgValue(const Arg& arg) const
{
    try {
        if (arg.defaultValue.type() == Variant::TYPE_BOOL) {
            auto value = m_hpr->argParser.get_option<popl::Value<bool>>(arg.longName);
            if (value) {
                return value->value();
            }
        } else if (arg.defaultValue.type() == Variant::TYPE_INT) {
            auto value = m_hpr->argParser.get_option<popl::Value<int>>(arg.longName);
            if (value) {
                return value->value();
            }
        } else if (arg.defaultValue.type() == Variant::TYPE_DOUBLE) {
            auto value = m_hpr->argParser.get_option<popl::Value<double>>(arg.longName);
            if (value) {
                return value->value();
            }
        } else if (arg.defaultValue.type() == Variant::TYPE_STRING) {
            auto value = m_hpr->argParser.get_option<popl::Value<std::string>>(arg.longName);
            if (value) {
                return value->value();
            }
        } else if (arg.defaultValue.type() == Variant::TYPE_NULL) {
            auto value = m_hpr->argParser.get_option<popl::Switch>(arg.longName);
            if (value) {
                return value->value();
            }
        } else {
            LOG_DEBUG("get arg not support");
        }
        return arg.defaultValue;
    } catch (std::invalid_argument error) {
        LOG_DEBUG(error.what());
        return arg.defaultValue;
    }
    return arg.defaultValue;
}

VariantMap Application::readConfig(const std::string& fileName)
{
    VariantMap data;
    std::string configPath = Utils::getCfgPath(fileName, "MIFSA_CONF_DIR", "mifsa");
    std::string expectPath = "/etc/" + fileName;
    if (expectPath.empty()) {
        LOG_WARNING("config data not exists");
        return data;
    }
    data = Variant::readJson(configPath);
    if (data.empty()) {
        LOG_WARNING("read config data error");
        return data;
    }
    return data;
}

void Application::parserFlag(int flag)
{
    if (flag & Application::CHECK_SINGLETON) {
        if (!Utils::programCheckSingleton(m_hpr->typeName)) {
            LOG_WARNING("program is already running");
            std::exit(1);
        }
    }
    if (flag & Application::CHECK_TERMINATE) {
        static Application* termIntance = this;
        Utils::programRegisterTerminate([](int reval) {
            termIntance->exit(reval);
        });
    }
}

void Application::parserArgs(const std::vector<Arg>& args)
{
    auto helpOpt = m_hpr->argParser.add<popl::Switch>("h", "help", "produce help message");
    for (const auto& arg : args) {
        if (arg.defaultValue.type() == Variant::TYPE_BOOL) {
            m_hpr->argParser.add<popl::Value<bool>>(arg.shortName, arg.longName, arg.description, arg.defaultValue.toBool());
        } else if (arg.defaultValue.type() == Variant::TYPE_INT) {
            m_hpr->argParser.add<popl::Value<int>>(arg.shortName, arg.longName, arg.description, arg.defaultValue.toInt());
        } else if (arg.defaultValue.type() == Variant::TYPE_DOUBLE) {
            m_hpr->argParser.add<popl::Value<double>>(arg.shortName, arg.longName, arg.description, arg.defaultValue.toDouble());
        } else if (arg.defaultValue.type() == Variant::TYPE_STRING) {
            m_hpr->argParser.add<popl::Value<std::string>>(arg.shortName, arg.longName, arg.description, arg.defaultValue.toString());
        } else if (arg.defaultValue.type() == Variant::TYPE_NULL) {
            m_hpr->argParser.add<popl::Switch>(arg.shortName, arg.longName, arg.description);
        } else {
            LOG_DEBUG("parser arg not support");
            continue;
        }
    }
    try {
        m_hpr->argParser.parse(argc(), argv());
    } catch (popl::invalid_option error) {
        LOG_DEBUG("parameter input error: ", error.what());
        LOG_DEBUG(m_hpr->argParser);
        std::exit(0);
    }
    if (helpOpt->is_set()) {
        LOG_DEBUG(m_hpr->argParser);
        std::exit(0);
    }
}

MIFSA_NAMESPACE_END
