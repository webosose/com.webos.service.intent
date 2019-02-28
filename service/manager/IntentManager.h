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

#ifndef MANAGER_INTENTMANAGER_H_
#define MANAGER_INTENTMANAGER_H_

#include <iostream>

#include <luna-service2/lunaservice.hpp>
#include <pbnjson.hpp>

#include "interface/IManageable.h"

using namespace std;
using namespace LS;
using namespace pbnjson;

class IntentManager : public Handle, public IManageable<IntentManager> {
friend class IManageable<IntentManager>;
public:
    virtual ~IntentManager();

    virtual bool onInitialization() override;
    virtual bool onFInalization() override;

    bool launch(LSMessage &message);
    bool finish(LSMessage &message);
    bool resolve(LSMessage &message);
    bool getHandler(LSMessage &message);
    bool setHandler(LSMessage &message);
    bool registerHandler(LSMessage &message);
    bool unregisterHandler(LSMessage &message);

private:
    static const string NAME;
    IntentManager();

    void pre(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void post(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
};

#endif /* MANAGER_INTENTMANAGER_H_ */
