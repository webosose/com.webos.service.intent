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
#include "ApplicationManager.h"

#include "manager/IntentManager.h"
#include "manager/HandlerManager.h"
#include "util/Logger.h"

ApplicationManager::ApplicationManager()
{
    setClassName("ApplicationManager");
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

bool ApplicationManager::launch(Intent& intent, Handler& handler)
{
    static const string API = "luna://com.webos.service.applicationmanager/launch";
    pbnjson::JValue requestPayload = pbnjson::Object();
    requestPayload.put("id", handler.getId());

    pbnjson::JValue params = pbnjson::Object();
    params.put("requester", intent.getRequester());
    params.put("action", intent.getAction());
    params.put("uri", intent.getUri().toString());
    if (!intent.getExtra().isNull())
        params.put("extra", intent.getExtra());
    requestPayload.put("params", params);

    try {
        auto call = IntentManager::getInstance().callOneReply(
            API.c_str(),
            requestPayload.stringify().c_str()
        );
        auto reply = call.get(5000);
    }
    catch (const LS::Error &e) {
        Logger::error(getClassName(), __FUNCTION__, e.what());
    }
    return true;
}

bool ApplicationManager::_listApps(LSHandle* sh, LSMessage* reply, void* ctx)
{
    Message response(reply);
    pbnjson::JValue responsePayload = JDomParser::fromString(response.getPayload());

    Logger::info(ApplicationManager::getInstance().getClassName(), "Return", "listApps");
    if (response.isHubError() || !responsePayload["returnValue"].asBool()) {
        Logger::error(ApplicationManager::getInstance().getClassName(), __FUNCTION__, std::string(response.getPayload()));
        return false;
    }

    if (!responsePayload.hasKey("apps") || !responsePayload["apps"].isArray() || responsePayload["apps"].arraySize() <= 0) {
        Logger::error(ApplicationManager::getInstance().getClassName(), __FUNCTION__, response.getPayload());
        return false;
    }

    string id;
    for (JValue application : responsePayload["apps"].items()) {
        if (!application.hasKey("id") || application["id"].asString(id) != CONV_OK) {
            Logger::warning(ApplicationManager::getInstance().getClassName(), __FUNCTION__, "'id' is empty");
            continue;
        }
        if (!application.hasKey("intentFilter")) {
            Logger::debug(ApplicationManager::getInstance().getClassName(), __FUNCTION__, "'intentFilter' is null");
            continue;
        }

        Handler handler;
        handler.fromJson(application["intentFilter"]);
        handler.setId(application["id"].asString());
        handler.setType(HandlerType_AppInfo);
        if (!HandlerManager::getInstance().registerHandler(handler)) {
            Logger::error(ApplicationManager::getInstance().getClassName(), handler.getId(), "Failed to register handler");
        }
    }

    // Ready when first *running* subscription.
    if (!ApplicationManager::getInstance().isReady()) {
        ApplicationManager::getInstance().ready();
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
        Logger::debug(getClassName(), "Call", "listApps");
        m_listApps.continueWith(_listApps, this);
    }
    catch (const LS::Error &e) {
        Logger::error(getClassName(), __FUNCTION__, e.what());
    }
}
