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

#include "ConfFile.h"

#include <fstream>

#include "util/Logger.h"

const string ConfFile::PATH_DATABASE = "/var/preferences/com.webos.service.intent.json";

ConfFile::ConfFile()
    : m_testmode(false)
{
    setClassName("ConfigManager");
}

ConfFile::~ConfFile()
{
}

bool ConfFile::onInitialization()
{
    const char* testfile = std::getenv("TESTFILE");

    if (testfile != nullptr) {
        m_database = JDomParser::fromFile(testfile);
    }
    if (!m_database.isNull()) {
        m_testmode = true;
        Logger::info(getClassName(), __FUNCTION__, "TEST MODE");
    } else {
        m_database = JDomParser::fromFile(PATH_DATABASE.c_str());
        m_testmode = false;
        Logger::info(getClassName(), __FUNCTION__, "NORMAL MODE");
    }
    if (m_database.isNull()) {
        Logger::info(getClassName(), __FUNCTION__, "The database file is empty.");
        m_database = pbnjson::Object();
    }
    return true;
}

bool ConfFile::onFinalization()
{
    if (!m_testmode) {
        ofstream out(PATH_DATABASE.c_str());
        if (!out.is_open()) {
            Logger::error(getClassName(), __FUNCTION__, "Failed to open database file");
            return true;
        }
        Logger::info(getClassName(), __FUNCTION__, "Database is saved");
        out << m_database.stringify("    ");
        out.close();
    }
    return true;
}

JValue& ConfFile::get()
{
    return m_database;
}
