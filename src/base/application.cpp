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
#include "hpplib/popl.hpp"
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

Variant Application::getArgValue(const Arg& arg, const std::string& configName) const
{
    try {
        if (arg._defaultValue.type() == Variant::TYPE_NULL) {
            auto value = m_hpr->argParser.get_option<popl::Switch>(arg._longName);
            if (value && value->is_set()) {
                return value->value();
            }
        } else if (arg._defaultValue.type() == Variant::TYPE_BOOL) {
            auto value = m_hpr->argParser.get_option<popl::Value<bool>>(arg._longName);
            if (value && value->is_set()) {
                return value->value();
            }
        } else if (arg._defaultValue.type() == Variant::TYPE_INT) {
            auto value = m_hpr->argParser.get_option<popl::Value<int>>(arg._longName);
            if (value && value->is_set()) {
                return value->value();
            }
        } else if (arg._defaultValue.type() == Variant::TYPE_DOUBLE) {
            auto value = m_hpr->argParser.get_option<popl::Value<double>>(arg._longName);
            if (value && value->is_set()) {
                return value->value();
            }
        } else if (arg._defaultValue.type() == Variant::TYPE_STRING) {
            auto value = m_hpr->argParser.get_option<popl::Value<std::string>>(arg._longName);
            if (value && value->is_set()) {
                return value->value();
            }
        } else {
            LOG_WARNING("get arg not support");
        }
        if (!configName.empty()) {
            if (m_hpr->config.contains(configName)) {
                return m_hpr->config.value(configName);
            }
        }
    } catch (std::invalid_argument error) {
        return arg._defaultValue;
    }
    return arg._defaultValue;
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
    std::string errorString;
    data = Variant::readJson(configPath, &errorString);
    if (data.empty() && !errorString.empty()) {
        LOG_WARNING("read config data error:", errorString);
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
            return;
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
        if (arg._defaultValue.type() == Variant::TYPE_NULL) {
            m_hpr->argParser.add<popl::Switch>(arg._shortName, arg._longName, arg._description);
        } else if (arg._defaultValue.type() == Variant::TYPE_BOOL) {
            m_hpr->argParser.add<popl::Value<bool>>(arg._shortName, arg._longName, arg._description, arg._defaultValue.toBool());
        } else if (arg._defaultValue.type() == Variant::TYPE_INT) {
            m_hpr->argParser.add<popl::Value<int>>(arg._shortName, arg._longName, arg._description, arg._defaultValue.toInt());
        } else if (arg._defaultValue.type() == Variant::TYPE_DOUBLE) {
            m_hpr->argParser.add<popl::Value<double>>(arg._shortName, arg._longName, arg._description, arg._defaultValue.toDouble());
        } else if (arg._defaultValue.type() == Variant::TYPE_STRING) {
            m_hpr->argParser.add<popl::Value<std::string>>(arg._shortName, arg._longName, arg._description, arg._defaultValue.toString());
        } else {
            LOG_WARNING("parser arg not support");
            continue;
        }
    }
    try {
        m_hpr->argParser.parse(argc(), argv());
    } catch (popl::invalid_option error) {
        LOG_WARNING("parameter input error: ", error.what());
        LOG_DEBUG(m_hpr->argParser);
        std::exit(0);
        return;
    }
    for (const auto& non_option_arg : m_hpr->argParser.non_option_args()) {
        LOG_WARNING("non_option_args: ", non_option_arg);
    }
    for (const auto& unknown_option_args : m_hpr->argParser.unknown_options()) {
        LOG_WARNING("unknown_option_args: ", unknown_option_args);
    }
    if (helpOpt->is_set()) {
        LOG_DEBUG(m_hpr->argParser);
        std::exit(0);
        return;
    }
}

MIFSA_NAMESPACE_END
