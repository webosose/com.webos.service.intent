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

#include "SAM.h"

#include "core/IntentManager.h"
#include "core/MainDaemon.h"
#include "base/Handler.h"
#include "base/Handlers.h"
#include "base/Intents.h"
#include "conf/ConfFile.h"
#include "util/Logger.h"

const string& SAM::CLASS_NAME = "SAM";

SAM::SAM(const string& sessionId)
    : AbsLunaClient("com.webos.service.applicationmanager"),
      m_listApps(0)
{
    setClassName("SAM");
    m_sessionId = sessionId;
}

SAM::~SAM()
{
    if (m_listApps != 0) {
        try {
            LSCallCancel(IntentManager::getInstance().get(), m_listApps, nullptr);
        } catch(LS::Error& err) {
            Logger::error(CLASS_NAME, __FUNCTION__, "Exception in LS2");
        }
        m_listApps = 0;
    }
}

void SAM::onServerStatusChanged(bool isConnected)
{
    static string method = string("luna://") + m_name + string("/listApps");

    if (isConnected) {
        if (m_listApps == 0) {
            LSErrorSafe error;
#if defined(WEBOS_TARGET_DISTRO_WEBOS_AUTO)
            if (!LSCallSession(
#else
            if (!LSCall(
#endif
                    IntentManager::getInstance().get(),
                    method.c_str(),
                    AbsLunaClient::getSubscriptionPayload().stringify().c_str(),
#if defined(WEBOS_TARGET_DISTRO_WEBOS_AUTO)
                    m_sessionId.c_str(),
#endif
                    onListApps,
                    this,
                    &m_listApps,
                    &error
            )) {
                Logger::error(CLASS_NAME, __FUNCTION__, error.message);
            }
        }
    } else {
        if (m_listApps != 0) {
            LSCallCancel(IntentManager::getInstance().get(), m_listApps, nullptr);
            m_listApps = 0;
        }

        Handlers::getInstance().removeBySessionId(m_sessionId);
        Logger::info(CLASS_NAME, __FUNCTION__, m_sessionId, "Apps are removed");
    }
}

bool SAM::onListApps(LSHandle* sh, LSMessage* reply, void* ctx)
{
    Message response(reply);
    string sessionId = AbsLunaClient::getSessionId(reply);
    JValue subscriptionPayload = JDomParser::fromString(response.getPayload());
    bool returnValue = false;
    JValueUtil::getValue(subscriptionPayload, "returnValue", returnValue);
    // Let's avoid too many logs
    // Logger::logSubscriptionResponse(CLASS_NAME, __FUNCTION__, response, subscriptionPayload);

    if (response.isHubError() || !returnValue) {
        Logger::error(CLASS_NAME, __FUNCTION__, std::string(response.getPayload()));
        return true;
    }

    JValue apps;
    if (!JValueUtil::getValue(subscriptionPayload, "apps", apps) || !apps.isArray()) {
        Logger::error(CLASS_NAME, __FUNCTION__, "Failed to get 'apps' in subscriptionPayload");
        return true;
    }

    for (JValue app : apps.items()) {
        string id = "";
        JValue intentFilters;

        if (!JValueUtil::getValue(app, "id", id)) {
            Logger::warning(CLASS_NAME, __FUNCTION__, "'id' is empty");
            continue;
        }

        if (!JValueUtil::getValue(app, "intentFilters", intentFilters)) {
            ConfFile::getInstance().getIntentFilter(id, intentFilters);
        }

        HandlerPtr handler = make_shared<Handler>();
        handler->setName(id);
        handler->setSessionId(sessionId);
        handler->setIntentFilters(intentFilters);
        Handlers::getInstance().add(handler);
    }

    return true;
}

int SAM::launch(IntentPtr intent, HandlerPtr handler)
{
    static string method = string("luna://") + m_name + string("/launch");
    pbnjson::JValue requestPayload = pbnjson::Object();
    pbnjson::JValue params = pbnjson::Object();

    intent->toJson(params);
    params.put("from", intent->getOwner());
    params.put("intentId", intent->getIntentId());
    
    requestPayload.put("params", params);
    requestPayload.put("id", handler->getName());

    LSErrorSafe error;
    LSMessageToken token = 0;
    Logger::logCallRequest(getClassName(), __FUNCTION__, method, requestPayload);

#if defined(WEBOS_TARGET_DISTRO_WEBOS_AUTO)
    if (!LSCallSessionOneReply(
#else
    if (!LSCallOneReply(
#endif
        IntentManager::getInstance().get(),
        method.c_str(),
        requestPayload.stringify().c_str(),
#if defined(WEBOS_TARGET_DISTRO_WEBOS_AUTO)
        m_sessionId.c_str(),
#endif
        onLaunch,
        nullptr,
        &token,
        &error
    )) {
        Logger::error(CLASS_NAME, __FUNCTION__, error.message);
        return -1;
    }
    return token;
}

bool SAM::onLaunch(LSHandle* sh, LSMessage* reply, void* ctx)
{
    Message response(reply);
    JValue subscriptionPayloadFrom = JDomParser::fromString(response.getPayload());
    JValue subscriptionPayloadTo = JDomParser::fromString(response.getPayload());

    Logger::logCallResponse(CLASS_NAME, __FUNCTION__, response, subscriptionPayloadFrom);

    int intentId = response.getResponseToken();

    IntentPtr intent = Intents::getInstance().get(intentId);
    if (intent == nullptr) {
        Logger::error(CLASS_NAME, __FUNCTION__, "Cannot find intent");
        return true;
    }

    bool returnValue = false;
    JValueUtil::getValue(subscriptionPayloadFrom, "returnValue", returnValue);

    if (!returnValue) {
        subscriptionPayloadTo.put("returnValue", returnValue);
        subscriptionPayloadTo.put("subscribed", true);
        subscriptionPayloadTo.put("intentId", intentId);
        subscriptionPayloadTo.put("errorText", "Failed to start intent");
        intent->respond(subscriptionPayloadTo.stringify());

        Intents::getInstance().remove(intentId);
    }
    return true;
}
