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

#include "SessionManager.h"

#include "bus/session/SAM.h"

SessionManager::SessionManager()
    : AbsLunaClient("com.webos.service.sessionmanager")
{
    setClassName("SessionManager");
}

SessionManager::~SessionManager()
{
    m_getSessionList.cancel();
}

void SessionManager::onServerStatusChanged(bool isConnected)
{
    static string method = string("luna://") + m_name + string("/getSessionList");

    m_getSessionList.cancel();
    m_sessions.clear();
    if (isConnected) {
        m_getSessionList = IntentManager::getInstance().callMultiReply(
            method.c_str(),
            AbsLunaClient::getSubscriptionPayload().stringify().c_str(),
            onGetSessionList,
            this
        );
    }
}

bool SessionManager::onGetSessionList(LSHandle* sh, LSMessage* reply, void* ctx)
{
    Message response(reply);
    JValue subscriptionPayload = JDomParser::fromString(response.getPayload());
    bool returnValue = false;

    JValueUtil::getValue(subscriptionPayload, "returnValue", returnValue);
    // Logger::logSubscriptionResponse(getInstance().getClassName(), __FUNCTION__, response, subscriptionPayload);

    if (response.isHubError() || !returnValue) {
        Logger::error(getInstance().getClassName(), __FUNCTION__, std::string(response.getPayload()));
        return true;
    }

    JValue sessionList;
    if (!JValueUtil::getValue(subscriptionPayload, "sessionList", sessionList) || !sessionList.isArray()) {
        Logger::error(getInstance().getClassName(), __FUNCTION__, "Failed to get 'sessionList' in subscriptionPayload");
        return true;
    }

    // mark as removed
    for (auto it = getInstance().m_sessions.begin(); it != getInstance().m_sessions.end(); ++it) {
        it->second->setRunning(false);
    }

    // update current status
    for (JValue session : sessionList.items()) {
        string sessionId = "";

        if (!JValueUtil::getValue(session, "sessionId", sessionId)) {
            Logger::warning(getInstance().getClassName(), __FUNCTION__, "'sessionId' is empty");
            continue;
        }

        if (getInstance().m_sessions.find(sessionId) == getInstance().m_sessions.end()) {
            getInstance().m_sessions[sessionId] = make_shared<Session>(sessionId, getInstance().m_mainloop);
        }
        getInstance().m_sessions[sessionId]->setRunning(true);
    }

    // handle removed session
    for (auto it = getInstance().m_sessions.cbegin(); it != getInstance().m_sessions.cend() ;) {
        if (!it->second->isRunning()) {
            it = getInstance().m_sessions.erase(it);
        } else {
            ++it;
        }
    }
    return true;
}
