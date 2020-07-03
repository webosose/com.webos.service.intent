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

#ifndef BUS_HOST_SESSIONMANAGER_H_
#define BUS_HOST_SESSIONMANAGER_H_

#include <iostream>
#include <map>

#include <luna-service2/lunaservice.hpp>

#include "bus/AbsLunaClient.h"
#include "bus/session/Session.h"
#include "interface/ISingleton.h"

using namespace std;

class SessionManager : public AbsLunaClient,
                       public ISingleton<SessionManager> {
friend class ISingleton<SessionManager>;
public:
    virtual ~SessionManager();

    SessionPtr getSession(const string& sessionId)
    {
        if (m_sessions.find(sessionId) == m_sessions.end()) {
            return nullptr;
        }
        return m_sessions[sessionId];
    }

protected:
    virtual bool onInitialization() override;
    virtual bool onFinalization() override;
    virtual void onServerStatusChanged(bool isConnected) override;

private:
    static bool onGetSessionList(LSHandle* sh, LSMessage* reply, void* ctx);

    SessionManager();

    Call m_getSessionList;

    map<string, SessionPtr> m_sessions;
};

#endif /* BUS_HOST_SESSIONMANAGER_H_ */
