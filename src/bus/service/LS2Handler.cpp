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

#include <base/Handler.h>
#include <base/Intent.h>
#include <bus/client/SAM.h>
#include <bus/service/IntentManager.h>
#include <bus/service/LS2Handler.h>
#include <conf/ConfigFile.h>
#include <lunaservice.h>

#include "util/Logger.h"

const string LS2Handler::NAME = "com.webos.service.intent";
const LSMethod LS2Handler::METHODS[] = {
    { "launch",             LS2Handler::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "finish",             LS2Handler::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "resolve",            LS2Handler::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "getHandler",         LS2Handler::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "setHandler",         LS2Handler::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "registerHandler",    LS2Handler::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { "unregisterHandler",  LS2Handler::onAPICalled, LUNA_METHOD_FLAGS_NONE },
    { 0,                    0,                        LUNA_METHOD_FLAGS_NONE }
};

void LS2Handler::writelog(LS::Message& request, const string& type, JValue& payload)
{
    string header = type + "-" + request.getMethod();
    string body = request.getSenderServiceName() ? request.getSenderServiceName() : request.getApplicationID();
    if (ConfigFile::getInstance().isVerbose()) {
        body += "\n" + payload.stringify("    ");
    }
    Logger::info(LS2Handler::getInstance().getClassName(), header, body);
}

LS2Handler::LS2Handler()
    : Handle(LS::registerService(NAME.c_str()))
{
    setClassName("IntentManager");
    this->registerCategory("/", METHODS, NULL, NULL);
}

LS2Handler::~LS2Handler()
{
}

bool LS2Handler::onInitialization()
{
    attachToLoop(m_mainloop);
    this->ready();
    return true;
}

bool LS2Handler::onFinalization()
{
    detach();
    return true;
}

void LS2Handler::launch(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
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
    if (!IntentManager::getInstance().launch(intent)) {
        responsePayload.put("errorText", "Failed to launch intent");
        return;
    }
}

void LS2Handler::finish(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    Intent intent;
    if (intent.fromJson(requestPayload) == false) {
        responsePayload.put("errorText", "Invalid parameter");
        return;
    }
}

void LS2Handler::resolve(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    Intent intent;
    if (!intent.fromJson(requestPayload)) {
        responsePayload.put("errorText", "Invalid parameter");
        return;
    }
    JValue handlers;

    if (requestPayload.objectSize() == 0) {
        handlers = IntentManager::getInstance().getAllHandlers();
    } else {
        handlers = IntentManager::getInstance().resolve(intent);
    }
    responsePayload.put("handlers", handlers);
}

void LS2Handler::getHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
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
    responsePayload = IntentManager::getInstance().getHandler(handler.getId());

    if (responsePayload.objectSize() == 0) {
        responsePayload.put("errorText", "Failed to find handler");
    }
}

void LS2Handler::setHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
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
    if (!IntentManager::getInstance().setHandler(handler)) {
        responsePayload.put("errorText", "Failed to set handler");
        return;
    }
}

void LS2Handler::registerHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
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
    if (!IntentManager::getInstance().registerHandler(handler)) {
        responsePayload.put("errorText", "Failed to register handler");
        return;
    }
}

void LS2Handler::unregisterHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
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
    if (!IntentManager::getInstance().unregisterHandler(handler)) {
        responsePayload.put("errorText", "Failed to unregister handler");
        return;
    }
}

bool LS2Handler::onAPICalled(LSHandle *sh, LSMessage *msg, void *category_context)
{
    // All LS2 requests are handled in queue
    LS2Handler::getInstance().m_requests.emplace(msg);

    // Ignore onRequest's return values.
    onRequest(nullptr);
    return true;
}

gboolean LS2Handler::onRequest(gpointer user_data)
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
        Logger::warning(LS2Handler::getInstance().getClassName(), __FUNCTION__, "ConfigManager is not ready");
        isReady = false;
    }
    if (!IntentManager::getInstance().isReady()) {
        Logger::warning(LS2Handler::getInstance().getClassName(), __FUNCTION__, "HandlerManager is not ready");
        isReady = false;
    }
    if (!LS2Handler::getInstance().isReady()) {
        Logger::warning(LS2Handler::getInstance().getClassName(), __FUNCTION__, "IntentManager is not ready");
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
    while (LS2Handler::getInstance().m_requests.size() != 0) {
        // pop a request
        LS::Message request = LS2Handler::getInstance().m_requests.front();
        LS2Handler::getInstance().m_requests.pop();

        // pre processing before request handling
        pre(request, requestPayload, responsePayload);
        string method = request.getMethod();

        if (method == "launch") {
            LS2Handler::getInstance().launch(request, requestPayload, responsePayload);
        } else if (method == "finish") {
            LS2Handler::getInstance().finish(request, requestPayload, responsePayload);
        } else if (method == "resolve") {
            LS2Handler::getInstance().resolve(request, requestPayload, responsePayload);
        } else if (method == "getHandler") {
            LS2Handler::getInstance().getHandler(request, requestPayload, responsePayload);
        } else if (method == "setHandler") {
            LS2Handler::getInstance().setHandler(request, requestPayload, responsePayload);
        } else if (method == "registerHandler") {
            LS2Handler::getInstance().registerHandler(request, requestPayload, responsePayload);
        } else if (method == "unregisterHandler") {
            LS2Handler::getInstance().unregisterHandler(request, requestPayload, responsePayload);
        } else {
            responsePayload.put("errorText", "Please extend API handlers");
        }
        post(request, requestPayload, responsePayload);
    }
    pending = false;
    return G_SOURCE_REMOVE;
}

void LS2Handler::pre(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    requestPayload = JDomParser::fromString(request.getPayload());
    responsePayload = pbnjson::Object();

    request.getPayload();
    request.getSender();
    request.getKind();

    writelog(request, "Request", requestPayload);
}

void LS2Handler::post(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
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
