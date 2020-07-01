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
    : AbsLunaClient("com.webos.service.applicationmanager")
{
    setClassName("SAM");
    m_sessionId = sessionId;
}

SAM::~SAM()
{
    m_listApps.cancel();
}

void SAM::onServerStatusChanged(bool isConnected)
{
    static string method = string("luna://") + m_name + string("/listApps");

    m_listApps.cancel();
    if (isConnected) {
        m_listApps = IntentManager::getInstance().callMultiReply(
            method.c_str(),
            AbsLunaClient::getSubscriptionPayload().stringify().c_str(),
            onListApps,
            this,
            nullptr,
            m_sessionId.empty() ? nullptr : m_sessionId.c_str()
        );
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
    // Logger::logSubscriptionResponse(getInstance().getClassName(), __FUNCTION__, response, subscriptionPayload);

    if (response.isHubError() || !returnValue) {
        Logger::error(CLASS_NAME, __FUNCTION__, std::string(response.getPayload()));
        return true;
    }

    JValue apps;
    if (!JValueUtil::getValue(subscriptionPayload, "apps", apps) || !apps.isArray()) {
        Logger::error(CLASS_NAME, __FUNCTION__, "Failed to get 'apps' in subscriptionPayload");
        return true;
    }

    for (JValue application : apps.items()) {
        string id = "";
        JValue intentFilters;

        if (!JValueUtil::getValue(application, "id", id)) {
            Logger::warning(CLASS_NAME, __FUNCTION__, "'id' is empty");
            continue;
        }

        if (!JValueUtil::getValue(application, "intentFilters", intentFilters)) {
            ConfFile::getInstance().getIntentFilter(id, intentFilters);
        }

        HandlerPtr handler = make_shared<Handler>();
        handler->setName(id);
        handler->setSessionId(sessionId);
        handler->setIntentFilters(intentFilters);
        if (!Handlers::getInstance().add(handler)) {
            Logger::error(CLASS_NAME, handler->getName(), "Failed to register handler");
        }
    }

    return true;
}

int SAM::launch(IntentPtr intent, HandlerPtr handler)
{
    static string method = string("luna://") + m_name + string("/launch");
    pbnjson::JValue requestPayload = pbnjson::Object();
    pbnjson::JValue params = pbnjson::Object();

    intent->toJson(params);
    requestPayload.put("params", params);
    requestPayload.put("id", handler->getName());

    LSErrorSafe error;
    LSMessageToken token = 0;
    Logger::logCallRequest(getClassName(), __FUNCTION__, method, requestPayload);
    if (!LSCallSessionOneReply(
        IntentManager::getInstance().get(),
        method.c_str(),
        requestPayload.stringify().c_str(),
        m_sessionId.c_str(),
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
