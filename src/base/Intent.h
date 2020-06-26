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

#ifndef BASE_INTENT_H_
#define BASE_INTENT_H_

#include <iostream>

#include <luna-service2++/message.hpp>

#include "base/Uri.h"
#include "interface/ISerializable.h"
#include "interface/IClassName.h"

using namespace std;

typedef shared_ptr<LS::Message> MessagePtr;

class Intent : public ISerializable {
public:
    Intent();
    virtual ~Intent();

    // ISerializable
    virtual bool fromJson(const JValue& json) override;
    virtual bool toJson(JValue& json) override;

    void setIntentId(const int& intentId)
    {
        m_intentId = intentId;
    }
    const int getIntentId()
    {
        return m_intentId;
    }

    void setName(const string& name)
    {
        m_name = name;
    }
    const string& getName()
    {
        return m_name;
    }

    void setAction(const string& action)
    {
        m_action = action;
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

    const string& getResult()
    {
        return m_result;
    }

    const Uri& getUri() const
    {
        return m_uri;
    }
    bool hasUri()
    {
        return !m_uri.empty();
    }

    void setOwner(const string& owner)
    {
        m_owner = owner;
    }
    const string& getOwner()
    {
        return m_owner;
    }

    void addSubscriber(MessagePtr subscriber)
    {
        m_subscribers.push_back(subscriber);
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
        for (auto it = m_subscribers.begin(); it != m_subscribers.end(); ++it) {
            (*it)->respond(responsePayload.c_str());
        }
        return true;
    }

private:
    const static string CLASS_NAME;

    int m_intentId;
    string m_name;
    string m_action;
    string m_mimeType;

    string m_result;
    JValue m_extra;

    Uri m_uri;

    string m_owner;
    deque<MessagePtr> m_subscribers;
};

typedef shared_ptr<Intent> IntentPtr;

#endif /* BASE_INTENT_H_ */
