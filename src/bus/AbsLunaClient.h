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

#ifndef BUS_ABSLUNACLIENT_H_
#define BUS_ABSLUNACLIENT_H_

#include <iostream>
#include <boost/signals2.hpp>
#include <luna-service2/lunaservice.hpp>
#include <pbnjson.hpp>

#include "core/IntentManager.h"
#include "interface/IClassName.h"
#include "interface/IInitializable.h"
#include "util/JValueUtil.h"

using namespace std;
using namespace LS;
using namespace pbnjson;

class LSErrorSafe: public LSError {
public:
    LSErrorSafe()
    {
        LSErrorInit(this);
    }

    ~LSErrorSafe()
    {
        LSErrorFree(this);
    }
};

class AbsLunaClient : public IInitializable{
public:
    static const string getSessionId(LSMessage* message)
    {
        if (LSMessageGetSessionId(message) == nullptr)
            return "";
        return LSMessageGetSessionId(message);
    }

    static JValue& getEmptyPayload()
    {
        static JValue _PAYLOAD;

        if (_PAYLOAD.isNull()) {
            _PAYLOAD = pbnjson::Object();
        }
        return _PAYLOAD;
    }

    static JValue& getSubscriptionPayload()
    {
        static JValue _PAYLOAD;
        if (_PAYLOAD.isNull()) {
            _PAYLOAD = pbnjson::Object();
            _PAYLOAD.put("subscribe", true);
        }
        return _PAYLOAD;
    }

    AbsLunaClient(const string& name);
    virtual ~AbsLunaClient();

    boost::signals2::signal<void(bool)> EventServiceStatusChanged;

protected:
    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

    virtual void onServerStatusChanged(bool isConnected) = 0;

    string m_name;
    string m_sessionId;

    bool m_isConnected;

private:
    static bool _onServerStatusChanged(LSHandle* sh, LSMessage* message, void* context);

    Call m_statusCall;

};

#endif /* BUS_ABSLUNACLIENT_H_ */
