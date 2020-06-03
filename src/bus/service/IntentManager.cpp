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

bool IntentManager::launch(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    HandlerPtr handler = nullptr;
    IntentPtr intent = make_shared<Intent>();

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (intent->fromJson(requestPayload) == false) {
        errorText = "Invalid parameter";
        goto Done;
    }
    intent->setRequester(request.getSenderServiceName() ? request.getSenderServiceName() : request.getApplicationID());
    if (!intent->isValid()) {
        errorText = "Intent is not valid";
        goto Done;
    }
    if (intent->chooser()) {
        handler = Handlers::getInstance().getChooser();
    } else {
        handler = Handlers::getInstance().getHandler(intent);
    }
    if (handler == nullptr) {
        errorText = "Cannot find handler";
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
    if (intent.fromJson(requestPayload) == false) {
        errorText = "Invalid parameter";
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

    JValue handlers = pbnjson::Array();

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (requestPayload.objectSize() == 0) {
        Handlers::getInstance().toJson(handlers);
    } else {
        IntentPtr intent = make_shared<Intent>();
        intent->fromJson(requestPayload);
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
    if (!JValueUtil::getValue(requestPayload, "id", id)) {
        errorText = "Cannot find 'id' parameter";
        goto Done;
    }
    if (Handlers::getInstance().getHandler(id) == nullptr) {
        errorText = "Cannot find handler";
        goto Done;
    }
    if (!Handlers::getInstance().toJsonByID(responsePayload, id)) {
        errorText = "Cannot convert handler to json";
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

    HandlerPtr handler = nullptr;
    string id = "";
    int priority = 0;
    JValue actions;
    JValue mimeTypes;
    JValue uris;

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);
    if (!JValueUtil::getValue(requestPayload, "id", id)) {
        errorText = "'id' is required parameter";
        goto Done;
    }

    handler = Handlers::getInstance().getHandler(id);
    if (handler == nullptr) {
        errorText = "Cannot find proper handler";
        goto Done;
    } else if (handler->getType() == "appinfo") {
        errorText = "Updating 'appinfo' type is not allowed";
        goto Done;
    }

    if (JValueUtil::getValue(requestPayload, "priority", priority)) {
        handler->setPriority(priority);
    }
    if (JValueUtil::getValue(requestPayload, "actions", actions)) {
        handler->setActions(actions);
    }
    if (JValueUtil::getValue(requestPayload, "mimeTypes", mimeTypes)) {
        handler->setMimeTypes(mimeTypes);
    }
    if (JValueUtil::getValue(requestPayload, "uris", uris)) {
        handler->setUris(priority);
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

bool IntentManager::registerHandler(LSMessage &message)
{
    Message request(&message);
    JValue requestPayload = JDomParser::fromString(request.getPayload());
    JValue responsePayload = pbnjson::Object();
    string errorText = "";

    Logger::logAPIRequest(getInstance().getClassName(), __FUNCTION__, request, requestPayload);

    HandlerPtr newHandler = make_shared<Handler>();
    HandlerPtr oldHandler = nullptr;
    if (newHandler == nullptr) {
        errorText = "Cannot create handler object";
        goto Done;
    }
    if (!newHandler->fromJson(requestPayload)) {
        errorText = "Cannot parse requestPayload";
        goto Done;
    }
    newHandler->setType("runtime");
    if (!newHandler->isValid()) {
        errorText = "Invalid parameters";
        goto Done;
    }
    oldHandler = Handlers::getInstance().getHandler(newHandler->getId());
    if (oldHandler != nullptr) {
        if (oldHandler->getType() == "runtime") {
            errorText = "Already registered. Call 'updateHandler' API instead";
            goto Done;
        } else {
            errorText = "'appinfo' handler is exist";
            goto Done;
        }
    }
    if (!Handlers::getInstance().add(newHandler, "runtime")) {
        errorText = "Failed to register handler";
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
    if (!JValueUtil::getValue(requestPayload, "id", id)) {
        errorText = "'id' is required parameter";
        goto Done;
    }
    if (Handlers::getInstance().getHandler(id) == nullptr) {
        errorText = "Cannot find handler";
        goto Done;
    }
    if (!Handlers::getInstance().remove(id, "runtime")) {
        errorText = "Cannot unregister appinfo handler";
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
