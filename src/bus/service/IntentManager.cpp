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

#include <lunaservice.h>

#include "base/Handler.h"
#include "base/Handlers.h"
#include "base/Intent.h"
#include "bus/client/SAM.h"
#include "bus/service/IntentManager.h"
#include "conf/ConfigFile.h"
#include "util/Logger.h"

const string IntentManager::NAME = "com.webos.service.intent";
const LSMethod IntentManager::METHODS[] = {
    { "launch",             IntentManager::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "finish",             IntentManager::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "resolve",            IntentManager::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "getHandler",         IntentManager::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "setHandler",         IntentManager::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "registerHandler",    IntentManager::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "unregisterHandler",  IntentManager::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { 0,                    0,                        LUNA_METHOD_FLAGS_NONE }
};

void IntentManager::writelog(LS::Message& request, const string& type, JValue& payload)
{
    string header = type + "-" + request.getMethod();
    string body = request.getSenderServiceName() ? request.getSenderServiceName() : request.getApplicationID();
    if (ConfigFile::getInstance().isVerbose()) {
        body += "\n" + payload.stringify("    ");
    }
    Logger::info(IntentManager::getInstance().getClassName(), header, body);
}

IntentManager::IntentManager()
    : Handle(LS::registerService(NAME.c_str()))
{
    setClassName("IntentManager");
    this->registerCategory("/", METHODS, NULL, NULL);
}

IntentManager::~IntentManager()
{
}

bool IntentManager::onInitialization()
{
    attachToLoop(m_mainloop);
    this->ready();
    return true;
}

bool IntentManager::onFinalization()
{
    detach();
    return true;
}

void IntentManager::launch(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    Intent intent;
    if (intent.fromJson(requestPayload) == false) {
        responsePayload.put("errorText", "Invalid parameter");
        return;
    }
    if (!intent.checkAction()) {
        responsePayload.put("errorText", "'action' is required parameter");
        return;
    }
    if (!intent.checkUri()) {
        responsePayload.put("errorText", "'uri' is required parameter");
        return;
    }
    intent.setRequester(request.getSenderServiceName() ? request.getSenderServiceName() : request.getApplicationID());
    HandlerPtr handler = nullptr;
    if (intent.chooser()) {
        handler = Handlers::getInstance().getChooser();
    } else {
        handler = Handlers::getInstance().getHandler(intent);
    }
    if (handler == nullptr) {
        responsePayload.put("errorText", "Cannot find handler");
        return;
    }
    Logger::info(getClassName(), handler->getId(), "Launch target handler");
    SAM::getInstance().launch(intent, handler);
}

void IntentManager::finish(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    Intent intent;
    if (intent.fromJson(requestPayload) == false) {
        responsePayload.put("errorText", "Invalid parameter");
        return;
    }
}

void IntentManager::resolve(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    Intent intent;
    if (!intent.fromJson(requestPayload)) {
        responsePayload.put("errorText", "Invalid parameter");
        return;
    }
    JValue handlers = pbnjson::Array();

    if (requestPayload.objectSize() == 0) {
        Handlers::getInstance().toJson(handlers);
    } else {
        Handlers::getInstance().toJson(handlers, intent);
    }
    responsePayload.put("handlers", handlers);
}

void IntentManager::getHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    string id = "";
    if (!JValueUtil::getValue(requestPayload, "id", id)) {
        responsePayload.put("errorText", "'id' is required parameter");
        return;
    }
    if (Handlers::getInstance().getHandler(id) == nullptr) {
        responsePayload.put("errorText", "Cannot find handler");
        return;
    }
    if (!Handlers::getInstance().toJson(responsePayload, id)) {
        responsePayload.put("errorText", "Failed to find handler");
        return;
    }
}

void IntentManager::updateHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    Handler handler;
    if (!handler.fromJson(requestPayload)) {
        responsePayload.put("errorText", "Invalid parameter");
        return;
    }
    if (handler.getId().empty()) {
        responsePayload.put("errorText", "'id' is required parameter");
        return;
    }
    if (handler.getActions().size() == 0) {
        responsePayload.put("errorText", "'actions' is required parameter");
        return;
    }
    handler.setType(HandlerType_Runtime);
//    if (!IntentManager::getInstance().setHandler(handler)) {
//        responsePayload.put("errorText", "Failed to set handler");
//        return;
//    }
}

void IntentManager::registerHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    HandlerPtr handler = make_shared<Handler>();
    if (handler == nullptr) {
        responsePayload.put("errorText", "Invalid parameter");
        return;
    }
    if (handler->fromJson(requestPayload)) {
        responsePayload.put("errorText", "Invalid parameter");
        return;
    }
    if (Handlers::getInstance().add(handler, HandlerType_Runtime)) {
        responsePayload.put("errorText", "Invalid parameter");
        return;
    }
}

void IntentManager::unregisterHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    string id = "";
    if (!JValueUtil::getValue(requestPayload, "id", id)) {
        responsePayload.put("errorText", "'id' is required parameter");
        return;
    }
    if (Handlers::getInstance().getHandler(id) == nullptr) {
        responsePayload.put("errorText", "Cannot find handler");
        return;
    }
    if (!Handlers::getInstance().remove(id, HandlerType_Runtime)) {
        responsePayload.put("errorText", "Cannot unregister appinfo handler");
        return;
    }
}

bool IntentManager::onAPICalled(LSHandle *sh, LSMessage *msg, void *category_context)
{
    // All LS2 requests are handled in queue
    IntentManager::getInstance().m_requests.emplace(msg);

    // Ignore onRequest's return values.
    onRequest(nullptr);
    return true;
}

gboolean IntentManager::onRequest(gpointer user_data)
{
    // Not allowed recursive request handling To avoid unexpected behavior
    static bool pending = false;
    if (pending) {
        return G_SOURCE_REMOVE;
    }
    bool isReady = true;
//    if (!SAM::getInstance().isReady()) {
//        Logger::warning(LS2Handler::getInstance().getClassName(), __FUNCTION__, "ApplicationManager is not ready");
//        isReady = false;
//    }
    if (!ConfigFile::getInstance().isReady()) {
        Logger::warning(IntentManager::getInstance().getClassName(), __FUNCTION__, "ConfigManager is not ready");
        isReady = false;
    }
    if (!IntentManager::getInstance().isReady()) {
        Logger::warning(IntentManager::getInstance().getClassName(), __FUNCTION__, "HandlerManager is not ready");
        isReady = false;
    }
    if (!IntentManager::getInstance().isReady()) {
        Logger::warning(IntentManager::getInstance().getClassName(), __FUNCTION__, "IntentManager is not ready");
        isReady = false;
    }
    if (!isReady) {
        // Call me again
        g_timeout_add_seconds(1, onRequest, nullptr);
        return G_SOURCE_REMOVE;
    }

    // start API handling
    pending = true;
    JValue requestPayload, responsePayload;
    while (IntentManager::getInstance().m_requests.size() != 0) {
        // pop a request
        LS::Message request = IntentManager::getInstance().m_requests.front();
        IntentManager::getInstance().m_requests.pop();

        // pre processing before request handling
        pre(request, requestPayload, responsePayload);
        string method = request.getMethod();

        if (method == "launch") {
            IntentManager::getInstance().launch(request, requestPayload, responsePayload);
        } else if (method == "finish") {
            IntentManager::getInstance().finish(request, requestPayload, responsePayload);
        } else if (method == "resolve") {
            IntentManager::getInstance().resolve(request, requestPayload, responsePayload);
        } else if (method == "getHandler") {
            IntentManager::getInstance().getHandler(request, requestPayload, responsePayload);
        } else if (method == "updateHandler") {
            IntentManager::getInstance().updateHandler(request, requestPayload, responsePayload);
        } else if (method == "registerHandler") {
            IntentManager::getInstance().registerHandler(request, requestPayload, responsePayload);
        } else if (method == "unregisterHandler") {
            IntentManager::getInstance().unregisterHandler(request, requestPayload, responsePayload);
        } else {
            responsePayload.put("errorText", "Please extend API handlers");
        }
        post(request, requestPayload, responsePayload);
    }
    pending = false;
    return G_SOURCE_REMOVE;
}

void IntentManager::pre(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    requestPayload = JDomParser::fromString(request.getPayload());
    responsePayload = pbnjson::Object();

    request.getPayload();
    request.getSender();
    request.getKind();

    writelog(request, "Request", requestPayload);
}

void IntentManager::post(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    if (responsePayload.hasKey("errorText")) {
        Logger::warning(NAME, __FUNCTION__, responsePayload["errorText"].asString());
        responsePayload.put("returnValue", false);
    }
    if (!responsePayload.hasKey("returnValue")) {
        responsePayload.put("returnValue", true);
    }
    request.respond(responsePayload.stringify().c_str());

    writelog(request, "Response", responsePayload);
}
