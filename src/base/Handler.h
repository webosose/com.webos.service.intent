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

#ifndef BASE_HANDLER_H_
#define BASE_HANDLER_H_

#include <deque>
#include <iostream>

#include "base/Intent.h"
#include "base/Uri.h"
#include "interface/ISerializable.h"

using namespace std;

class Handler : public ISerializable {
friend class Handlers;
public:
    static string convertKey(const string& sessionId, const string& name);

    Handler();
    virtual ~Handler();

    virtual bool isMatched(IntentPtr intent);

    // ISerializable
    virtual bool toJson(JValue& json) override;

    string getKey()
    {
        return convertKey(m_sessionId, m_name);
    }

    void setName(string name)
    {
        m_name = std::move(name);
    }
    const string& getName() const
    {
        return m_name;
    }
    bool hasName()
    {
        return !m_name.empty();
    }

    void setSessionId(const string& sessionId)
    {
        m_sessionId = sessionId;
    }
    const string& getSessionId()
    {
        return m_sessionId;
    }

    void setIntentFilters(JValue& intentFilters)
    {
        if (!intentFilters.isNull() && intentFilters.isValid()) {
            m_intentFilters = intentFilters.duplicate();
        }
    }
    const JValue& getIntentFilters() const
    {
        return m_intentFilters;
    }
    bool hasIntentFilters()
    {
        return (m_intentFilters.arraySize() != 0);
    }

private:
    // ISerializable
    virtual bool fromJson(const JValue& json) override { return false; };

    bool compareIntentFilter(JValue& intentFilter, IntentPtr intent);
    bool compareString(JValue& actions, const string& action);
    bool compareUri(JValue& uris, const Uri& b);

    const static string CLASS_NAME;

    string m_name;
    string m_sessionId;
    JValue m_intentFilters;
};

typedef shared_ptr<Handler> HandlerPtr;

#endif /* BASE_HANDLER_H_ */
