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

#include "IntentManager.h"
#include "util/Logger.h"

const string IntentManager::NAME = "com.webos.service.intent";

IntentManager::IntentManager()
    : Handle(LS::registerService(NAME.c_str()))
{
    setName(NAME);

    LS_CATEGORY_BEGIN(IntentManager, "/")
        LS_CATEGORY_METHOD(launch)
        LS_CATEGORY_METHOD(finish)
        LS_CATEGORY_METHOD(resolve)
        LS_CATEGORY_METHOD(getHandler)
        LS_CATEGORY_METHOD(setHandler)
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
    return true;
}

bool IntentManager::onFInalization()
{
    detach();
    return true;
}

bool IntentManager::launch(LSMessage &message)
{
    LS::Message request(&message);
    JValue requestPayload, responsePayload;

    pre(request, requestPayload, responsePayload);
    post(request, requestPayload, responsePayload);
    return true;
}

bool IntentManager::finish(LSMessage &message)
{
    LS::Message request(&message);
    JValue requestPayload, responsePayload;

    pre(request, requestPayload, responsePayload);
    post(request, requestPayload, responsePayload);
    return true;
}

bool IntentManager::resolve(LSMessage &message)
{
    LS::Message request(&message);
    JValue requestPayload, responsePayload;

    pre(request, requestPayload, responsePayload);
    post(request, requestPayload, responsePayload);
    return true;
}

bool IntentManager::getHandler(LSMessage &message)
{
    LS::Message request(&message);
    JValue requestPayload, responsePayload;

    pre(request, requestPayload, responsePayload);
    post(request, requestPayload, responsePayload);
    return true;
}

bool IntentManager::setHandler(LSMessage &message)
{
    LS::Message request(&message);
    JValue requestPayload, responsePayload;

    pre(request, requestPayload, responsePayload);
    post(request, requestPayload, responsePayload);
    return true;
}

bool IntentManager::registerHandler(LSMessage &message)
{
    LS::Message request(&message);
    JValue requestPayload, responsePayload;

    pre(request, requestPayload, responsePayload);
    post(request, requestPayload, responsePayload);
    return true;
}

bool IntentManager::unregisterHandler(LSMessage &message)
{
    LS::Message request(&message);
    JValue requestPayload, responsePayload;

    pre(request, requestPayload, responsePayload);
    post(request, requestPayload, responsePayload);
    return true;
}

void IntentManager::pre(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    requestPayload = JDomParser::fromString(request.getPayload());
    responsePayload = pbnjson::Object();

    string msg = "";
    Logger::append(msg, "Request");
    Logger::append(msg, request.getMethod());
    Logger::append(msg, request.getSenderServiceName());
    Logger::normal(msg, NAME);
}

void IntentManager::post(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    if (responsePayload.hasKey("errorText")) {
        responsePayload.put("returnValue", false);
    }
    if (!responsePayload.hasKey("returnValue")) {
        responsePayload.put("returnValue", true);
    }
    request.respond(responsePayload.stringify().c_str());

    string msg = "";
    Logger::append(msg, "Response");
    Logger::append(msg, request.getMethod());
    Logger::append(msg, request.getSenderServiceName());
    Logger::normal(msg, NAME);
}
