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

#include "IntentManager.h"

#include <lunaservice.h>

#include "base/Handler.h"
#include "base/Handlers.h"
#include "base/Intent.h"
#include "base/Intents.h"
#include "bus/client/SAM.h"
#include "util/Logger.h"

bool IntentManager::_onServerStatusChanged(LSHandle* sh, LSMessage* message, void* context)
{
    Message response(message);
    JValue subscriptionPayload = JDomParser::fromString(response.getPayload());

    if (subscriptionPayload.isNull())
        return true;

    bool connected = false;
    if (!JValueUtil::getValue(subscriptionPayload, "connected", connected)) {
        return true;
    }

    cout << subscriptionPayload.stringify("    ") << endl;

    if (connected == false) {
        LSCallCancel(getInstance().get(), response.getMessageToken(), nullptr);
    }
    return true;
}

string IntentManager::getName(LS::Message& request)
{
    string result;
    if (request.getApplicationID() != nullptr) {
        result = request.getApplicationID();
    } else if (request.getSenderServiceName() != nullptr) {
        result = request.getSenderServiceName();
    } else {
        result = "";
    }
    return result;
}

IntentManager::IntentManager()
    : Handle(LS::registerService("com.webos.service.intent"))
{
    setClassName("IntentManager");

    LS_CATEGORY_BEGIN(IntentManager, "/")
        LS_CATEGORY_METHOD(query)
        LS_CATEGORY_METHOD(start)
        LS_CATEGORY_METHOD(sendResult)
        LS_CATEGORY_METHOD(subscribeResult)
    LS_CATEGORY_END
}

IntentManager::~IntentManager()
{
}

bool IntentManager::onInitialization()
{
    try {
        Handle::attachToLoop(m_mainloop);
    } catch(exception& e) {
    }
    return true;
}

bool IntentManager::onFinalization()
{
    return true;
}

bool IntentManager::query(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    JValue handlers = pbnjson::Array();
    IntentPtr intent = make_shared<Intent>();

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (requestPayload.isObject() && requestPayload.objectSize() == 0) {
        Handlers::getInstance().toJson(handlers);
    } else {
        if (requestPayload.isNull() ||
           !requestPayload.hasKey("intent") || !intent->fromJson(requestPayload["intent"]) ||
           !intent->isValid()) {
            errorText = "Invalid parameter";
            goto Done;
        }
        Handlers::getInstance().toJson(handlers, intent);
    }
    responsePayload.put("handlers", handlers);

Done:
    if (!errorText.empty()) {
        responsePayload.put("returnValue", false);
        responsePayload.put("errorText", errorText);
    } else {
        responsePayload.put("returnValue", true);
    }
    request.respond(responsePayload.stringify().c_str());
    return true;
}

bool IntentManager::start(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    HandlerPtr handler = nullptr;
    IntentPtr intent = make_shared<Intent>();
    int intentId = -1;

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (!intent->fromJson(requestPayload["intent"]) || !intent->isValid()) {
        errorText = "Invalid parameter";
        goto Done;
    }
    // TODO, currently, we don't care about multiple handlers
    // In the future, chooser should be displayed
    handler = Handlers::getInstance().getHandler(intent);
    if (handler == nullptr) {
        errorText = "Cannot find handler";
        goto Done;
    }

    // TODO, currently, we don't care about services
    // In the future, native service should be called
    intentId = SAM::getInstance().launch(intent, handler);
    if (intentId == -1) {
        errorText = "Failed to start intent";
        goto Done;
    }

    intent->setIntentId(intentId);
    intent->setOwner(getName(request));
    Intents::getInstance().add(intent);
    subscribeStatus(intent->getOwner());

    responsePayload.put("intentId", intentId);

Done:
    if (!errorText.empty()) {
        responsePayload.put("returnValue", false);
        responsePayload.put("errorText", errorText);
    } else {
        responsePayload.put("returnValue", true);
    }
    request.respond(responsePayload.stringify().c_str());
    return true;
}

bool IntentManager::sendResult(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    JValue payload = pbnjson::Object();
    string errorText = "";

    IntentPtr intent = nullptr;
    int intentId = -1;
    string result = "";
    JValue extra;

    if (!JValueUtil::getValue(requestPayload, "intentId", intentId)) {
        errorText = "'intentId' is required parameter";
        goto Done;
    }
    intent = Intents::getInstance().get(intentId);
    if (intent == nullptr) {
        errorText = "Cannot find intent";
        goto Done;
    }

    if (JValueUtil::getValue(requestPayload, "result", result)) {
        payload.put("result", result);
    }
    if (JValueUtil::getValue(requestPayload, "extra", extra) && extra.isObject()) {
        payload.put("extra", extra);
    }
    if (payload.objectSize() == 0) {
        errorText = "Cannot find 'result' or 'extra'";
        goto Done;
    }
    payload.put("returnValue", true);
    payload.put("subscribed", true);
    payload.put("intentId", intentId);
    intent->respond(payload.stringify());

Done:
    if (!errorText.empty()) {
        responsePayload.put("returnValue", false);
        responsePayload.put("errorText", errorText);
    } else {
        responsePayload.put("returnValue", true);
    }
    responsePayload.put("intentId", intentId);
    request.respond(responsePayload.stringify().c_str());
    return true;
}

bool IntentManager::subscribeResult(LSMessage &message)
{
    MessagePtr request = make_shared<Message>();
    JValue requestPayload = JDomParser::fromString(request->getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    IntentPtr intent = nullptr;
    int intentId = -1;

    if (!request->isSubscription()) {
        errorText = "'subscribe' should be 'true'";
        goto Done;
    }
    if (!JValueUtil::getValue(requestPayload, "intentId", intentId)) {
        errorText = "'intentId' is required parameter";
        goto Done;
    }
    intent = Intents::getInstance().get(intentId);
    if (intent == nullptr) {
        errorText = "Cannot find intent";
        goto Done;
    }
    intent->addSubscriber(request);

Done:
    if (!errorText.empty()) {
        responsePayload.put("returnValue", false);
        responsePayload.put("errorText", errorText);
    } else {
        responsePayload.put("returnValue", true);
    }
    responsePayload.put("intentId", intentId);
    request->respond(responsePayload.stringify().c_str());
    return true;
}

void IntentManager::subscribeStatus(const string& name)
{
    JValue requestPayload = pbnjson::Object();
    requestPayload.put("serviceName", name);
    requestPayload.put("subscribe", true);

    LSCall(this->get(),
           "luna://com.webos.service.bus/signal/registerServerStatus",
           requestPayload.stringify().c_str(),
           _onServerStatusChanged,
           nullptr,
           nullptr,
           nullptr
    );
}
