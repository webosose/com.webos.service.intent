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

#include "StorageManager.h"

#include <fstream>

#include "util/Logger.h"

// TODO: following value should be defined in build env.
const string StorageManager::PATH = "/var/preferences/com.webos.service.intent.json";

StorageManager::StorageManager()
{
    setName("StorageManager");
}

StorageManager::~StorageManager()
{
}

bool StorageManager::onInitialization()
{
    m_database = JDomParser::fromFile(PATH.c_str());

    if (m_database.isNull()) {
        Logger::info(m_name, "The storage file is empty. Try to initialize the database");
        m_database = pbnjson::Object();
    }
    return true;
}

bool StorageManager::onFinalization()
{
    sync();
    return true;
}

JValue& StorageManager::get()
{
    return m_database;
}

void StorageManager::sync()
{
    ofstream out(PATH.c_str());
    if (!out.is_open()) {
        Logger::error(m_name, "Failed to open database file");
        return;
    }
    Logger::info(m_name, "database is syncronized");
    out << m_database.stringify("    ");
    out.close();
}
