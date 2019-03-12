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
    m_name = "com.webos.service.applicationmanager";
}

ApplicationManager::~ApplicationManager()
{
}

bool ApplicationManager::onInitialization()
{
    m_serverStatus = IntentManager::getInstance().registerServerStatus(m_name.c_str(), onServerStatus);
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

    if (response.isHubError() || !responsePayload["returnValue"].asBool()) {
        Logger::error("Error occurred: " + std::string(response.getPayload()), ApplicationManager::getInstance().m_name);
        return false;
    }

    if (!responsePayload.hasKey("apps") || !responsePayload["apps"].isArray() || responsePayload["apps"].arraySize() <= 0) {
        Logger::error("Error occurred: " + std::string(response.getPayload()), ApplicationManager::getInstance().m_name);
        return false;
    }

    string id;
    for (JValue application : responsePayload["apps"].items()) {
        if (!application.hasKey("intentFilter")) continue;
        if (!application.hasKey("id")) continue; // Unknown error!
        if (application["id"].asString(id) != CONV_OK) continue;

        Handler handler;
        handler.fromJson(application["intentFilter"]);
        handler.setId(application["id"].asString());
//        if (!HandlerManager::getInstance().registerHandler(handler)) {
//            Logger::error("Error occurred: ", ApplicationManager::getInstance().m_name);
//        }
    }
    return true;
}

void ApplicationManager::listApps()
{
    static const string API = "luna://" + m_name + "/listApps";
    pbnjson::JValue requestPayload = pbnjson::Object();
    requestPayload.put("subscribe", true);

    try {
        m_listApps = IntentManager::getInstance().callMultiReply(
            API.c_str(),
            requestPayload.stringify().c_str()
        );
        Logger::debug(API + " Request:" + requestPayload.stringify(), m_name);
        m_listApps.continueWith(_listApps, this);
    }
    catch (const LS::Error &e) {
        Logger::error("Exception: " + std::string(e.what()), m_name);
    }
}
