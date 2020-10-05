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

#include "bus/AbsLunaClient.h"

#include "conf/ConfFile.h"

bool AbsLunaClient::_onServerStatusChanged(LSHandle* sh, LSMessage* message, void* context)
{
    AbsLunaClient* self = static_cast<AbsLunaClient*>(context);

    Message response(message);
    JValue subscriptionPayload = JDomParser::fromString(response.getPayload());

    if (subscriptionPayload.isNull())
        return true;

    bool connected = false;
    if (!JValueUtil::getValue(subscriptionPayload, "connected", connected)) {
        return true;
    }
    string sessionId = "";
    if (!self->m_sessionId.empty()) {
        if (!JValueUtil::getValue(subscriptionPayload, "sessionId", sessionId)) {
            return true;
        } else if (sessionId != self->m_sessionId) {
            return true;
        }
    }

    if (connected)
        Logger::info(self->getClassName(), __FUNCTION__, self->m_sessionId, "Service is up");
    else
        Logger::info(self->getClassName(), __FUNCTION__, self->m_sessionId, "Service is down");

    self->m_isConnected = connected;
    self->EventServiceStatusChanged(connected);
    self->onServerStatusChanged(connected);
    return true;
}

AbsLunaClient::AbsLunaClient(const string& name)
    : m_name(name),
      m_isConnected(false),
      m_statusCall(0)
{
    setClassName("AbsLunaClient");
}

AbsLunaClient::~AbsLunaClient()
{
    if (m_statusCall != 0) {
        try {
            LSCallCancel(IntentManager::getInstance().get(), m_statusCall, nullptr);
        } catch(LS::Error& err) {
            Logger::error(getClassName(), __FUNCTION__, "Exception in LS2");
        }
        m_statusCall = 0;
    }
}

bool AbsLunaClient::onInitialization()
{
    JValue requestPayload = pbnjson::Object();
    requestPayload.put("serviceName", m_name);
    if (!m_sessionId.empty())
        requestPayload.put("sessionId", m_sessionId);

    LSErrorSafe error;
    if (!LSCall(
            IntentManager::getInstance().get(),
            "luna://com.webos.service.bus/signal/registerServerStatus",
            requestPayload.stringify().c_str(),
            _onServerStatusChanged,
            this,
            &m_statusCall,
            &error
    )) {
        Logger::error(getClassName(), __FUNCTION__, error.message);
        return false;
    }
    return true;
}

bool AbsLunaClient::onFinalization()
{
    if (m_statusCall != 0) {
        LSCallCancel(IntentManager::getInstance().get(), m_statusCall, nullptr);
        m_statusCall = 0;
    }
    return true;
}
