// Copyright (c) 2020-2021 LG Electronics, Inc.
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

#ifndef BASE_INTENT_H_
#define BASE_INTENT_H_

#include <iostream>
#include <luna-service2++/message.hpp>
#include <luna-service2++/subscription.hpp>

#include "base/Uri.h"
#include "interface/IClassName.h"
#include "interface/ISerializable.h"

using namespace std;

typedef shared_ptr<LS::Message> MessagePtr;

class Intent : public ISerializable {
public:
    Intent();
    virtual ~Intent();

    // ISerializable
    virtual bool fromJson(const JValue& json) override;
    virtual bool toJson(JValue& json) override;

    const string& getName()
    {
        return m_name;
    }
    bool hasName()
    {
        return !m_name.empty();
    }

    const string& getAction() const
    {
        return m_action;
    }
    bool hasAction()
    {
        return !m_action.empty();
    }

    const string& getMimeType() const
    {
        return m_mimeType;
    }
    bool hasMimeType()
    {
        return !m_mimeType.empty();
    }

    const JValue& getExtra()
    {
        return m_extra;
    }

    const Uri& getUri() const
    {
        return m_uri;
    }
    bool hasUri()
    {
        return !m_uri.empty();
    }

    const int& getIntentId()
    {
        return m_intentId;
    }

    void setResult(const string& result)
    {
        m_result = result;
    }
    const string& getResult()
    {
        return m_result;
    }

    void setSessionId(const string& sessionId)
    {
        m_sessionId = sessionId;
    }
    const string& getSessionId()
    {
        return m_sessionId;
    }
    bool hasSessionId()
    {
        return !m_sessionId.empty();
    }

    void setToken(const int token)
    {
        m_token = token;
    }
    const int getToken()
    {
        return m_token;
    }

    void setOwner(const string& owner)
    {
        m_owner = owner;
    }
    const string& getOwner()
    {
        return m_owner;
    }

    void addSubscriber(LS::Message& request)
    {
        m_subscriptionPoint.subscribe(request);
    }

    bool isValid()
    {
        if (!m_name.empty()) {
            return true;
        }
        if (!m_action.empty())
            return true;
        return false;
    }

    bool respond(const string& responsePayload)
    {
        return m_subscriptionPoint.post(responsePayload.c_str());
    }

private:
    const static string CLASS_NAME;
    static int s_counter;

    // Intent inner
    string m_name;
    string m_action;
    string m_mimeType;
    Uri m_uri;
    JValue m_extra;

    // Intent outer
    int m_intentId;
    string m_result;
    string m_sessionId;
    string m_owner;
    int m_token;

    // runtime values
    LS::SubscriptionPoint m_subscriptionPoint;
};

typedef shared_ptr<Intent> IntentPtr;

#endif /* BASE_INTENT_H_ */
