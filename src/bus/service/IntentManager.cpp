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

IntentManager::IntentManager()
    : Handle(LS::registerService("com.webos.service.intent"))
{
    setClassName("IntentManager");

    LS_CATEGORY_BEGIN(IntentManager, "/")
        LS_CATEGORY_METHOD(launch)
        LS_CATEGORY_METHOD(finish)
        LS_CATEGORY_METHOD(resolve)
        LS_CATEGORY_METHOD(getHandler)
        LS_CATEGORY_METHOD(updateHandler)
        LS_CATEGORY_METHOD(registerHandler)
        LS_CATEGORY_METHOD(unregisterHandler)
    LS_CATEGORY_END
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

bool IntentManager::launch(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    HandlerPtr handler = nullptr;
    Intent intent;

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (requestPayload.isNull()) {
        errorText = "invalid parameters";
        goto Done;
    }
    if (intent.fromJson(requestPayload) == false) {
        responsePayload.put("errorText", "Invalid parameter");
        goto Done;
    }
    if (!intent.checkAction()) {
        responsePayload.put("errorText", "'action' is required parameter");
        goto Done;
    }
    if (!intent.checkUri()) {
        responsePayload.put("errorText", "'uri' is required parameter");
        goto Done;
    }
    intent.setRequester(request.getSenderServiceName() ? request.getSenderServiceName() : request.getApplicationID());
    if (intent.chooser()) {
        handler = Handlers::getInstance().getChooser();
    } else {
        handler = Handlers::getInstance().getHandler(intent);
    }
    if (handler == nullptr) {
        responsePayload.put("errorText", "Cannot find handler");
        goto Done;
    }
    Logger::info(getClassName(), handler->getId(), "Launch target handler");
    SAM::getInstance().launch(intent, handler);

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

bool IntentManager::finish(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    Intent intent;

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (requestPayload.isNull()) {
        errorText = "invalid parameters";
        goto Done;
    }
    if (intent.fromJson(requestPayload) == false) {
        responsePayload.put("errorText", "Invalid parameter");
        goto Done;
    }

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

bool IntentManager::resolve(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    Intent intent;
    JValue handlers = pbnjson::Array();

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (requestPayload.isNull()) {
        errorText = "invalid parameters";
        goto Done;
    }
    if (!intent.fromJson(requestPayload)) {
        responsePayload.put("errorText", "Invalid parameter");
        goto Done;
    }
    if (requestPayload.objectSize() == 0) {
        Handlers::getInstance().toJson(handlers);
    } else {
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

bool IntentManager::getHandler(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    string id = "";

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (requestPayload.isNull()) {
        errorText = "invalid parameters";
        goto Done;
    }
    if (!JValueUtil::getValue(requestPayload, "id", id)) {
        responsePayload.put("errorText", "'id' is required parameter");
        goto Done;
    }
    if (Handlers::getInstance().getHandler(id) == nullptr) {
        responsePayload.put("errorText", "Cannot find handler");
        goto Done;
    }
    if (!Handlers::getInstance().toJson(responsePayload, id)) {
        responsePayload.put("errorText", "Failed to find handler");
        goto Done;
    }

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

bool IntentManager::updateHandler(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    Handler handler;

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (requestPayload.isNull()) {
        errorText = "invalid parameters";
        goto Done;
    }
    if (!handler.fromJson(requestPayload)) {
        responsePayload.put("errorText", "Invalid parameter");
        goto Done;
    }
    if (handler.getId().empty()) {
        responsePayload.put("errorText", "'id' is required parameter");
        goto Done;
    }
    if (handler.getActions().size() == 0) {
        responsePayload.put("errorText", "'actions' is required parameter");
        goto Done;
    }
    handler.setType(HandlerType_Runtime);
//    if (!IntentManager::getInstance().setHandler(handler)) {
//        responsePayload.put("errorText", "Failed to set handler");
//        return;
//    }




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

bool IntentManager::registerHandler(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    HandlerPtr handler = make_shared<Handler>();

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (requestPayload.isNull()) {
        errorText = "invalid parameters";
        goto Done;
    }
    if (handler == nullptr) {
        responsePayload.put("errorText", "Invalid parameter");
        goto Done;
    }
    if (handler->fromJson(requestPayload)) {
        responsePayload.put("errorText", "Invalid parameter");
        goto Done;
    }
    if (Handlers::getInstance().add(handler, HandlerType_Runtime)) {
        responsePayload.put("errorText", "Invalid parameter");
        goto Done;
    }






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

bool IntentManager::unregisterHandler(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    string id = "";

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (requestPayload.isNull()) {
        errorText = "invalid parameters";
        goto Done;
    }
    if (!JValueUtil::getValue(requestPayload, "id", id)) {
        responsePayload.put("errorText", "'id' is required parameter");
        goto Done;
    }
    if (Handlers::getInstance().getHandler(id) == nullptr) {
        responsePayload.put("errorText", "Cannot find handler");
        goto Done;
    }
    if (!Handlers::getInstance().remove(id, HandlerType_Runtime)) {
        responsePayload.put("errorText", "Cannot unregister appinfo handler");
        goto Done;
    }

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
