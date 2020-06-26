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
#include "util/JValueUtil.h"

const string ConfFile::PATH_DATABASE = "/etc/palm/com.webos.service.intent.json";

ConfFile::ConfFile()
{
    setClassName("ConfigManager");
}

ConfFile::~ConfFile()
{
}

bool ConfFile::onInitialization()
{
    m_database = JDomParser::fromFile(PATH_DATABASE.c_str());
    if (m_database.isNull()) {
        Logger::info(getClassName(), __FUNCTION__, "The database file is empty.");
        m_database = pbnjson::Object();
    } else {
        Logger::info(getClassName(), __FUNCTION__, "The database file is loaded.");
    }
    return true;
}

bool ConfFile::onFinalization()
{
    ofstream out(PATH_DATABASE.c_str());
    if (!out.is_open()) {
        Logger::error(getClassName(), __FUNCTION__, "Failed to open database file");
        return true;
    }
    Logger::info(getClassName(), __FUNCTION__, "Database is saved");
    out << m_database.stringify("    ");
    out.close();
    return true;
}

bool ConfFile::getIntentFilter(const string& appId, JValue& intentFilter)
{
    if (!m_database.hasKey("handlers") || !m_database["handlers"].hasKey(appId))
        return false;
    if (!m_database["handlers"][appId].isArray())
        return false;
    intentFilter = m_database["handlers"][appId];
    return true;
}
