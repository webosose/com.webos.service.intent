/* @@@LICENSE
 *
 * Copyright (c) 2019 LG Electronics, Inc.
 *
 * Confidential computer software. Valid license from LG required for
 * possession, use or copying. Consistent with FAR 12.211 and 12.212,
 * Commercial Computer Software, Computer Software Documentation, and
 * Technical Data for Commercial Items are licensed to the U.S. Government
 * under vendor's standard commercial license.
 *
 * LICENSE@@@
 */

#include "ConfigManager.h"

#include <fstream>

#include "util/Logger.h"

const string ConfigManager::DEBUG_DB_PATH = "/etc/palm/com.webos.service.intent.json";
const string ConfigManager::RUNTIME_DB_PATH = "/var/preferences/com.webos.service.intent.json";

ConfigManager::ConfigManager()
    : m_isTest(false)
    , m_isVerbose(false)
{
    setClassName("ConfigManager");

    const char* test = std::getenv("TEST");
    const char* testfile = std::getenv("TESTFILE");
    const char* verbose = std::getenv("VERBOSE");

    if (test && strcmp(test, "true") == 0) {
        m_debugDB = JDomParser::fromFile(testfile);
        if (!m_debugDB.isNull()) {
            m_isTest = true;
        } else {
            Logger::error(getClassName(), "The test file is empty.");
            m_debugDB = pbnjson::Object();
        }
    }
    if (verbose && strcmp(verbose, "true") == 0) {
        Logger::getInstance().setLevel(LogLevel_VERBOSE);
        m_isVerbose = true;
    }
}

ConfigManager::~ConfigManager()
{
}

bool ConfigManager::onInitialization()
{
    m_runtimeDB = JDomParser::fromFile(RUNTIME_DB_PATH.c_str());
    if (m_runtimeDB.isNull()) {
        Logger::info(getClassName(), "The runtime file is empty. Try to initialize the database");
        m_runtimeDB = pbnjson::Object();
    }
    ready();
    return true;
}

bool ConfigManager::onFinalization()
{
    syncRuntime();
    return true;
}

JValue& ConfigManager::getDebugDB()
{
    return m_debugDB;
}

JValue& ConfigManager::getRuntimeDB()
{
    return m_runtimeDB;
}

void ConfigManager::syncRuntime()
{
    ofstream out(RUNTIME_DB_PATH.c_str());
    if (!out.is_open()) {
        Logger::error(getClassName(), "Failed to open database file");
        return;
    }
    Logger::info(getClassName(), "database is syncronized");
    out << m_runtimeDB.stringify("    ");
    out.close();
}
