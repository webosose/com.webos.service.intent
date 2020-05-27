// Copyright (c) 2020 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include "ConfigFile.h"

#include <fstream>

#include "util/Logger.h"

const string ConfigFile::DEBUG_DB_PATH = "/etc/palm/com.webos.service.intent.json";
const string ConfigFile::RUNTIME_DB_PATH = "/var/preferences/com.webos.service.intent.json";

ConfigFile::ConfigFile()
    : m_isTest(false),
      m_isVerbose(false)
{
    setClassName("ConfigManager");

    const char* test = std::getenv("TEST");
    const char* testfile = std::getenv("TESTFILE");

    if (test && strcmp(test, "true") == 0) {
        m_debugDB = JDomParser::fromFile(testfile);
        if (!m_debugDB.isNull()) {
            m_isTest = true;
        } else {
            Logger::error(getClassName(), __FUNCTION__, "The test file is empty.");
            m_debugDB = pbnjson::Object();
        }
    }
}

ConfigFile::~ConfigFile()
{
}

bool ConfigFile::onInitialization()
{
    m_runtimeDB = JDomParser::fromFile(RUNTIME_DB_PATH.c_str());
    if (m_runtimeDB.isNull()) {
        Logger::info(getClassName(), __FUNCTION__, "The runtime file is empty. Try to initialize the database");
        m_runtimeDB = pbnjson::Object();
    }
    ready();
    return true;
}

bool ConfigFile::onFinalization()
{
    syncRuntime();
    return true;
}

JValue& ConfigFile::getDebugDB()
{
    return m_debugDB;
}

JValue& ConfigFile::getRuntimeDB()
{
    return m_runtimeDB;
}

void ConfigFile::syncRuntime()
{
    ofstream out(RUNTIME_DB_PATH.c_str());
    if (!out.is_open()) {
        Logger::error(getClassName(), __FUNCTION__, "Failed to open database file");
        return;
    }
    Logger::info(getClassName(), __FUNCTION__, "database is syncronized");
    out << m_runtimeDB.stringify("    ");
    out.close();
}
