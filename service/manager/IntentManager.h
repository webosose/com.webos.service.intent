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
    static void writelog(LS::Message& request, const string& type, JValue& payload);

    virtual ~IntentManager();

    // IManageable
    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

    // APIs
    void launch(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void finish(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void resolve(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void getHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void setHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void registerHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    void unregisterHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload);

    static const string NAME;

private:
    static bool onRequest(LSHandle* sh, LSMessage* msg, void* category_context);
    static gboolean onRequest(gpointer user_data);

    static void pre(LS::Message& request, JValue& requestPayload, JValue& responsePayload);
    static void post(LS::Message& request, JValue& requestPayload, JValue& responsePayload);

    IntentManager();

    static const LSMethod METHODS[];

    queue<LS::Message> m_requests;

};

#endif /* MANAGER_INTENTMANAGER_H_ */
