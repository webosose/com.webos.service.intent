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

#ifndef BASE_HANDLER_H_
#define BASE_HANDLER_H_

#include <iostream>
#include <deque>

#include "base/Intent.h"
#include "base/Uri.h"
#include "interface/ISerializable.h"

using namespace std;

class Handler : public ISerializable {
friend class Handlers;
public:
    Handler();
    virtual ~Handler();

    virtual bool launch(Intent intent);
    virtual bool isMatched(IntentPtr intent);

    // ISerializable
    virtual bool toJson(JValue& json) override;

    void setName(string name)
    {
        m_name = name;
    }
    const string& getName() const
    {
        return m_name;
    }
    bool hasName()
    {
        return !m_name.empty();
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
    bool checkMime(JValue& mimeTypes, const string& mimeType);

    const static string CLASS_NAME;

    string m_name;
    JValue m_intentFilters;

    deque<IntentPtr> m_intents;
};

typedef shared_ptr<Handler> HandlerPtr;

#endif /* BASE_HANDLER_H_ */
