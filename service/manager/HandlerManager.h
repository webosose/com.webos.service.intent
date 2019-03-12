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

#ifndef MANAGER_HANDLERMANAGER_H_
#define MANAGER_HANDLERMANAGER_H_

#include <iostream>

#include "interface/IManageable.h"
#include "core/Handler.h"
#include "core/Intent.h"

using namespace std;

class HandlerManager : public IManageable<HandlerManager> {
friend class IManageable<HandlerManager>;
public:
    virtual ~HandlerManager();

    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

    virtual JValue resolve(Intent intent);
    virtual JValue getHandler(const string& id);
    virtual bool setHandler(Handler& handler);
    virtual bool registerHandler(Handler& handler);
    virtual bool unregisterHandler(Handler& handler);

private:
    HandlerManager();

    virtual bool hasHandler(const string& id);
    virtual deque<Handler>::iterator findHandler(const string& id);

    deque<Handler> m_handlers;

};

#endif /* MANAGER_HANDLERMANAGER_H_ */
