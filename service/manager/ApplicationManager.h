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

#ifndef MANAGER_APPLICATIONMANAGER_H_
#define MANAGER_APPLICATIONMANAGER_H_

#include <iostream>

#include <luna-service2/lunaservice.hpp>

#include "interface/IManageable.h"

using namespace std;

class ApplicationManager : public IManageable<ApplicationManager> {
friend class IManageable<ApplicationManager>;
public:
    virtual ~ApplicationManager();

    virtual bool onInitialization() override;
    virtual bool onFinalization() override;
    static bool onServerStatus(bool isConnected);

    bool launch();

private:
    ApplicationManager();

    static bool _listApps(LSHandle* sh, LSMessage* reply, void* ctx);
    void listApps();

    LS::ServerStatus m_serverStatus;
    LS::Call m_listApps;
};

#endif /* MANAGER_APPLICATIONMANAGER_H_ */
