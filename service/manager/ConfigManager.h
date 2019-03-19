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

#ifndef MANAGER_CONFIGMANAGER_H_
#define MANAGER_CONFIGMANAGER_H_

#include <iostream>

#include <pbnjson.hpp>

#include "interface/IManageable.h"

using namespace std;
using namespace pbnjson;

class ConfigManager : public IManageable<ConfigManager> {
friend class IManageable<ConfigManager>;
public:
    virtual ~ConfigManager();

    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

    virtual JValue& getDebugDB();
    virtual JValue& getRuntimeDB();
    virtual void syncRuntime();

    bool isTest()
    {
        return m_isTest;
    }

    bool isVerbose()
    {
        return m_isVerbose;
    }

private:
    static const string DEBUG_DB_PATH;
    static const string RUNTIME_DB_PATH;

    ConfigManager();

    JValue m_debugDB;
    JValue m_runtimeDB;

    bool m_isTest;
    bool m_isVerbose;
};

#endif /* MANAGER_CONFIGMANAGER_H_ */
