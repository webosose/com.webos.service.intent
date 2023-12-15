// Copyright (c) 2020-2024 LG Electronics, Inc.
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

#include "Account.h"

#include "bus/session/SAM.h"

Account::Account()
    : AbsLunaClient("com.webos.service.account")
{
    setClassName("Account");
}

Account::~Account()
{
    m_getSessions.cancel();
}

bool Account::onInitialization()
{
#if defined(ENABLE_SESSION)
    return AbsLunaClient::onInitialization();
#else
    // Create empty session
    m_sessions[""] = make_shared<Session>("", m_mainloop);
    return true;
#endif
}

bool Account::onFinalization()
{
    m_sessions.clear();
    return AbsLunaClient::onFinalization();
}

void Account::onServerStatusChanged(bool isConnected)
{
    static const string METHOD = string("luna://") + m_name + string("/getSessions");

    m_getSessions.cancel();
    m_sessions.clear();
    if (isConnected) {
        m_getSessions = IntentManager::getInstance().callMultiReply(
            METHOD.c_str(),
            AbsLunaClient::getSubscriptionPayload().stringify().c_str(),
            onGetSessions,
            this
        );
    }
}

bool Account::onGetSessions(LSHandle *sh, LSMessage *reply, void *ctx)
{
    Message response(reply);
    JValue subscriptionPayload = JDomParser::fromString(response.getPayload());
    bool returnValue = false;

    if (!JValueUtil::getValue(subscriptionPayload, "returnValue", returnValue) || !returnValue) {
        Logger::error(getInstance().getClassName(), __FUNCTION__, std::string(response.getPayload()));
        return true;
    }
    // Logger::logSubscriptionResponse(getInstance().getClassName(), __FUNCTION__,
    // response, subscriptionPayload);

    JValue sessions;
    if (!JValueUtil::getValue(subscriptionPayload, "sessions", sessions) || !sessions.isArray()) {
        Logger::error(getInstance().getClassName(), __FUNCTION__, "Failed to get 'sessions' in subscriptionPayload");
        return true;
    }

    // mark as removed
    for (auto it = getInstance().m_sessions.begin(); it != getInstance().m_sessions.end(); ++it) {
        it->second->setRunning(false);
    }

    // update current status
    string sessionId = "";
    for (JValue session : sessions.items()) {
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
    for (auto it = getInstance().m_sessions.cbegin();
         it != getInstance().m_sessions.cend();) {
        if (!it->second->isRunning()) {
            it = getInstance().m_sessions.erase(it);
        } else {
            ++it;
        }
    }
    return true;
}
