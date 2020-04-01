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
