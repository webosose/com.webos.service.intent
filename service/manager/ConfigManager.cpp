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
    setName("ConfigManager");

    const char* test = std::getenv("TEST");
    const char* verbose = std::getenv("VERBOSE");

    if (test && strcmp(test, "true") == 0) {
        m_isTest = true;
    }
    if (verbose && strcmp(verbose, "true") == 0) {
        m_isVerbose = true;
        Logger::getInstance().setLevel(LogLevel_VERBOSE);
    }
}

ConfigManager::~ConfigManager()
{
}

bool ConfigManager::onInitialization()
{
    m_debugDB = JDomParser::fromFile(DEBUG_DB_PATH.c_str());
    if (m_debugDB.isNull()) {
        Logger::info(m_name, "The buildtime file is empty.");
        m_debugDB = pbnjson::Object();
    }

    m_runtimeDB = JDomParser::fromFile(RUNTIME_DB_PATH.c_str());
    if (m_runtimeDB.isNull()) {
        Logger::info(m_name, "The runtime file is empty. Try to initialize the database");
        m_runtimeDB = pbnjson::Object();
    }
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
        Logger::error(m_name, "Failed to open database file");
        return;
    }
    Logger::info(m_name, "database is syncronized");
    out << m_runtimeDB.stringify("    ");
    out.close();
}
