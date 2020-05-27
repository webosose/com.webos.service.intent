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
#include <base/Handlers.h>
#include <bus/client/SAM.h>
#include <bus/service/IntentManager.h>
#include "util/Logger.h"

SAM::SAM()
    : AbsLunaClient("com.webos.service.applicationmanager")
{
    setClassName("ApplicationManager");
}

SAM::~SAM()
{
}

void SAM::onInitialzed()
{

}

void SAM::onFinalized()
{
    m_listApps.cancel();
}

void SAM::onServerStatusChanged(bool isConnected)
{
    static string method = string("luna://") + getName() + string("/listApps");
    if (isConnected) {
        m_listApps = IntentManager::getInstance().callMultiReply(
            method.c_str(),
            AbsLunaClient::getSubscriptionPayload().stringify().c_str(),
            _listApps,
            this
        );
    } else {
        m_listApps.cancel();
    }
}

bool SAM::launch(Intent& intent, HandlerPtr handler)
{
    static const string API = "luna://com.webos.service.applicationmanager/launch";
    pbnjson::JValue requestPayload = pbnjson::Object();
    requestPayload.put("id", handler->getId());

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

bool SAM::_listApps(LSHandle* sh, LSMessage* reply, void* ctx)
{
    Message response(reply);
    JValue subscriptionPayload = JDomParser::fromString(response.getPayload());
    Logger::logSubscriptionResponse(getInstance().getClassName(), __FUNCTION__, response, subscriptionPayload);

    if (response.isHubError() || !subscriptionPayload["returnValue"].asBool()) {
        Logger::error(SAM::getInstance().getClassName(), __FUNCTION__, std::string(response.getPayload()));
        return false;
    }

    if (!subscriptionPayload.hasKey("apps") ||
        !subscriptionPayload["apps"].isArray() ||
        subscriptionPayload["apps"].arraySize() <= 0) {
        Logger::error(SAM::getInstance().getClassName(), __FUNCTION__, response.getPayload());
        return false;
    }

    string id;
    for (JValue application : subscriptionPayload["apps"].items()) {
        if (!application.hasKey("id") || application["id"].asString(id) != CONV_OK) {
            Logger::warning(SAM::getInstance().getClassName(), __FUNCTION__, "'id' is empty");
            continue;
        }
        if (!application.hasKey("intentFilter")) {
            Logger::debug(SAM::getInstance().getClassName(), __FUNCTION__, "'intentFilter' is null");
            continue;
        }

        if (!application.hasKey("intentFilter")) continue;

        HandlerPtr handler = make_shared<Handler>();
        handler->fromJson(application["intentFilter"]);
        handler->setId(application["id"].asString());
        handler->setType(HandlerType_AppInfo);
        if (Handlers::getInstance().add(handler, HandlerType_AppInfo)) {
            Logger::error(SAM::getInstance().getClassName(), handler->getId(), "Failed to register handler");
        }
    }
    // Ready when first *running* subscription.
//    if (!SAM::getInstance().isReady()) {
//        SAM::getInstance().ready();
//    }
    return true;
}
