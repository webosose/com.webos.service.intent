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

#include "bus/session/SAM.h"
#include "bus/host/SessionManager.h"
#include "base/Handler.h"
#include "base/Handlers.h"
#include "base/Intent.h"
#include "base/Intents.h"
#include "util/Logger.h"

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
    string sessionId = AbsLunaClient::getSessionId(request.get());
    string errorText = "";

    JValue handlers = pbnjson::Array();
    IntentPtr intent = make_shared<Intent>();

    if (requestPayload.isNull()) {
        errorText = "Invalid parameter";
        goto Done;
    }

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    JValueUtil::getValue(requestPayload, "sessionId", sessionId);
    if (requestPayload.hasKey("intent")) {
        if (!intent->fromJson(requestPayload["intent"]) || !intent->isValid()) {
            errorText = "Failed to parse intent";
            goto Done;
        }
        intent->setSessionId(sessionId);
        Handlers::getInstance().toJson(handlers, intent);
    } else {
        Handlers::getInstance().toJson(handlers, sessionId);
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
    string sessionId = AbsLunaClient::getSessionId(request.get());
    string errorText = "";

    HandlerPtr handler = nullptr;
    IntentPtr intent = make_shared<Intent>();

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    JValueUtil::getValue(requestPayload, "sessionId", sessionId);
    SessionPtr session = SessionManager::getInstance().getSession(sessionId);
    if (session == nullptr) {
        errorText = "Cannot find session";
        goto Done;
    }
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
    if (session->getSAM().launch(intent, handler) == -1) {
        errorText = "Failed to start intent";
        goto Done;
    }

    intent->setSessionId(sessionId);
    intent->setOwner(getName(request));
    responsePayload.put("intentId", intent->getIntentId());
    if (!sessionId.empty())
        responsePayload.put("sessionId", sessionId);
    subscribeStatus(intent->getOwner(), sessionId);

    if (request.isSubscription()) {
        responsePayload.put("subscribed", true);
        intent->addSubscriber(request);
    }
    Intents::getInstance().add(intent);

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
    JValue subscriptionPayload = pbnjson::Object();
    string errorText = "";

    IntentPtr requestIntent = nullptr;
    JValue intent = pbnjson::Object();
    int intentId = -1;
    string result = "";
    JValue extra;

    if (!JValueUtil::getValue(requestPayload, "intentId", intentId)) {
        errorText = "'intentId' is required parameter";
        goto Done;
    }
    requestIntent = Intents::getInstance().get(intentId);
    if (requestIntent == nullptr) {
        errorText = "Cannot find intent";
        goto Done;
    }
    if (!JValueUtil::getValue(requestPayload, "result", result)) {
        errorText = "Cannot find 'result'";
        goto Done;
    }
    subscriptionPayload.put("result", result);

    if (JValueUtil::getValue(requestPayload, "intent", intent)) {
        subscriptionPayload.put("intent", intent);
    }

    subscriptionPayload.put("returnValue", true);
    subscriptionPayload.put("subscribed", true);
    subscriptionPayload.put("intentId", intentId);
    subscriptionPayload.put("from", getName(request));
    requestIntent->respond(subscriptionPayload.stringify());

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
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    IntentPtr intent = nullptr;
    int intentId = -1;

    if (!JValueUtil::getValue(requestPayload, "intentId", intentId)) {
        errorText = "'intentId' is required parameter";
        goto Done;
    }
    intent = Intents::getInstance().get(intentId);
    if (intent == nullptr) {
        errorText = "Cannot find intent";
        goto Done;
    }
    if (!request.isSubscription()) {
        errorText = "'subscribe' should be 'true'";
        goto Done;
    }
    intent->addSubscriber(request);
    responsePayload.put("subscribed", true);

Done:
    if (!errorText.empty()) {
        responsePayload.put("returnValue", false);
        responsePayload.put("errorText", errorText);
        responsePayload.put("subscribed", false);
    } else {
        responsePayload.put("returnValue", true);
    }
    responsePayload.put("intentId", intentId);
    request.respond(responsePayload.stringify().c_str());
    return true;
}

void IntentManager::subscribeStatus(const string& name, const string& sessionId)
{
    JValue requestPayload = pbnjson::Object();
    requestPayload.put("serviceName", name);
    requestPayload.put("subscribe", true);
    if (!sessionId.empty())
        requestPayload.put("sessionId", sessionId);

    LSCall(this->get(),
           "luna://com.webos.service.bus/signal/registerServerStatus",
           requestPayload.stringify().c_str(),
           _onServerStatusChanged,
           nullptr,
           nullptr,
           nullptr
    );
}

bool IntentManager::_onServerStatusChanged(LSHandle* sh, LSMessage* message, void* context)
{
    Message response(message);
    JValue subscriptionPayload = JDomParser::fromString(response.getPayload());

    if (subscriptionPayload.isNull())
        return true;

    Logger::logSubscriptionResponse(getInstance().getClassName(), __FUNCTION__, response, subscriptionPayload);

    bool connected = false;
    string serviceName = "";
    if (!JValueUtil::getValue(subscriptionPayload, "connected", connected) ||
        !JValueUtil::getValue(subscriptionPayload, "serviceName", serviceName)) {
        return true;
    }

    if (connected == false) {
        Intents::getInstance().removeByOwner(serviceName);
        LSCallCancel(getInstance().get(), response.getMessageToken(), nullptr);
    }
    return true;
}
