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
#include <queue>

#include <pbnjson.hpp>
#include <luna-service2/lunaservice.hpp>

#include "interface/IManageable.h"

using namespace std;
using namespace LS;
using namespace pbnjson;

class IntentManager : public Handle, public IManageable<IntentManager> {
friend class IManageable<IntentManager>;
public:
    virtual ~IntentManager();

    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

    void launch(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void finish(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void resolve(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void getHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void setHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void registerHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void unregisterHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload);

private:
    static const string NAME;
    static const LSMethod METHODS[];

    queue<LS::Message> m_requests;

    static bool onRequest(LSHandle* sh, LSMessage* msg, void* category_context);

    static void pre(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    static void post(LS::Message& request, JValue& requestPayload, JValue& responsePayload);

    IntentManager();
};

#endif /* MANAGER_INTENTMANAGER_H_ */
