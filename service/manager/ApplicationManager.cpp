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

#include "ApplicationManager.h"

#include "core/Handler.h"
#include "manager/IntentManager.h"
#include "manager/HandlerManager.h"
#include "util/Logger.h"

ApplicationManager::ApplicationManager()
{
    setName("ApplicationManager");
}

ApplicationManager::~ApplicationManager()
{
}

bool ApplicationManager::onInitialization()
{
    m_serverStatus = IntentManager::getInstance().registerServerStatus("com.webos.service.applicationmanager", onServerStatus);
    return true;
}

bool ApplicationManager::onFinalization()
{
    m_serverStatus.cancel();
    m_listApps.cancel();
    return true;
}

bool ApplicationManager::onServerStatus(bool isConnected)
{
    if (isConnected) {
        ApplicationManager::getInstance().listApps();
    } else {
        ApplicationManager::getInstance().m_listApps.cancel();
    }
    return true;
}

bool ApplicationManager::launch()
{
    return true;
}

bool ApplicationManager::_listApps(LSHandle* sh, LSMessage* reply, void* ctx)
{
    Message response(reply);
    pbnjson::JValue responsePayload = JDomParser::fromString(response.getPayload());

    Logger::info(ApplicationManager::getInstance().m_name, "Return", "listApps");
    if (response.isHubError() || !responsePayload["returnValue"].asBool()) {
        Logger::error(ApplicationManager::getInstance().m_name, std::string(response.getPayload()));
        return false;
    }

    if (!responsePayload.hasKey("apps") || !responsePayload["apps"].isArray() || responsePayload["apps"].arraySize() <= 0) {
        Logger::error(ApplicationManager::getInstance().m_name, response.getPayload());
        return false;
    }

    string id;
    for (JValue application : responsePayload["apps"].items()) {
        if (!application.hasKey("intentFilter")) continue;
        if (!application.hasKey("id")) continue;
        if (application["id"].asString(id) != CONV_OK) continue;

        Handler handler;
        handler.fromJson(application["intentFilter"]);
        handler.setId(application["id"].asString());
        handler.setType(HandlerType_AppInfo);
        if (!HandlerManager::getInstance().registerHandler(handler)) {
            Logger::error(ApplicationManager::getInstance().m_name, handler.getId(), "Failed to register handler");
        }
    }
    return true;
}

void ApplicationManager::listApps()
{
    static const string API = "luna://com.webos.service.applicationmanager/listApps";
    pbnjson::JValue requestPayload = pbnjson::Object();
    requestPayload.put("subscribe", true);

    try {
        m_listApps = IntentManager::getInstance().callMultiReply(
            API.c_str(),
            requestPayload.stringify().c_str()
        );
        Logger::debug(m_name, "Call", "listApps");
        m_listApps.continueWith(_listApps, this);
    }
    catch (const LS::Error &e) {
        Logger::error(m_name, e.what());
    }
}
