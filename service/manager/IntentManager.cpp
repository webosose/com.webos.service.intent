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

#include "core/Intent.h"
#include "core/Handler.h"
#include "manager/HandlerManager.h"
#include "lunaservice.h"
#include "util/Logger.h"

const string IntentManager::NAME = "com.webos.service.intent";
const LSMethod IntentManager::METHODS[] = {
    { "launch", IntentManager::onRequest , LUNA_METHOD_FLAGS_NONE },
    { "finish", IntentManager::onRequest , LUNA_METHOD_FLAGS_NONE },
    { "resolve", IntentManager::onRequest , LUNA_METHOD_FLAGS_NONE },
    { "getHandler", IntentManager::onRequest , LUNA_METHOD_FLAGS_NONE },
    { "setHandler", IntentManager::onRequest , LUNA_METHOD_FLAGS_NONE },
    { "registerHandler", IntentManager::onRequest , LUNA_METHOD_FLAGS_NONE },
    { "unregisterHandler", IntentManager::onRequest , LUNA_METHOD_FLAGS_NONE },
    { 0, 0 , LUNA_METHOD_FLAGS_NONE }
};

bool IntentManager::onRequest(LSHandle *sh, LSMessage *msg, void *category_context)
{
    static bool pending = false;

    // All LS2 requests are handled in queue
    IntentManager::getInstance().m_requests.emplace(msg);

    // Not allowed recursive request handling To avoid unexpected behavior
    if (pending) {
        return true;
    }

    JValue requestPayload, responsePayload;
    pending = true;
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
        } else if (method == "setHandler") {
            IntentManager::getInstance().setHandler(request, requestPayload, responsePayload);
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
    return true;
}

IntentManager::IntentManager()
    : Handle(LS::registerService(NAME.c_str()))
{
    setName("IntentManager");
    this->registerCategory("/", METHODS, NULL, NULL);
}

IntentManager::~IntentManager()
{
}

bool IntentManager::onInitialization()
{
    attachToLoop(m_mainloop);
    return true;
}

bool IntentManager::onFinalization()
{
    detach();
    return true;
}

void IntentManager::pre(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    requestPayload = JDomParser::fromString(request.getPayload());
    responsePayload = pbnjson::Object();

    Logger::info(NAME, "Request", request.getMethod());
}

void IntentManager::post(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
{
    if (responsePayload.hasKey("errorText")) {
        Logger::warning(NAME, responsePayload["errorText"].asString());
        responsePayload.put("returnValue", false);
    }
    if (!responsePayload.hasKey("returnValue")) {
        responsePayload.put("returnValue", true);
    }
    request.respond(responsePayload.stringify().c_str());

    Logger::info(NAME, "Response", request.getMethod());
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
    if (!HandlerManager::getInstance().launch(intent)) {
        responsePayload.put("errorText", "Failed to launch intent");
        return;
    }
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
    JValue handlers;

    if (requestPayload.objectSize() == 0) {
        handlers = HandlerManager::getInstance().getAllHandlers();
    } else {
        handlers = HandlerManager::getInstance().resolve(intent);
    }
    responsePayload.put("handlers", handlers);
}

void IntentManager::getHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
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
    responsePayload = HandlerManager::getInstance().getHandler(handler.getId());

    if (responsePayload.objectSize() == 0) {
        responsePayload.put("errorText", "Failed to find handler");
    }
}

void IntentManager::setHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
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
    if (!HandlerManager::getInstance().setHandler(handler)) {
        responsePayload.put("errorText", "Failed to set hander");
        return;
    }
}

void IntentManager::registerHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
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
    if (!HandlerManager::getInstance().registerHandler(handler)) {
        responsePayload.put("errorText", "Failed to register handler");
        return;
    }
}

void IntentManager::unregisterHandler(LS::Message& request, JValue& requestPayload, JValue& responsePayload)
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
    if (!HandlerManager::getInstance().unregisterHandler(handler)) {
        responsePayload.put("errorText", "Failed to unregister handler");
        return;
    }
}
