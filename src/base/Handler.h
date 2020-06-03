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
#include <map>

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
    virtual bool fromJson(const JValue& json) override;
    virtual bool toJson(JValue& json) override;

    void setId(string id)
    {
        m_id = id;
    }
    const string& getId() const
    {
        return m_id;
    }

    void setType(const string& type)
    {
        m_type = type;
    }
    const string& getType()
    {
        return m_type;
    }

    void setPriority(const int& priority)
    {
        m_priority = priority;
    }
    const int& getPriority()
    {
        return m_priority;
    }

    void setActions(const JValue& actions)
    {
        if (!actions.isArray())
            return;
        m_actions = actions.duplicate();
    }
    const JValue& getActions()
    {
        return m_actions;
    }

    void setMimeTypes(const JValue& mimeTypes)
    {
        if (!mimeTypes.isArray())
            return;
        m_mimeTypes = mimeTypes.duplicate();
    }
    const JValue& getMimeTypes()
    {
        return m_mimeTypes;
    }

    void setUris(const JValue& uris)
    {
        if (!uris.isArray())
            return;
        m_uris = uris.duplicate();
    }
    const JValue& getUris()
    {
        return m_uris;
    }

    const bool isValid()
    {
        if (m_id.empty() || m_type.empty() || m_priority < 0 || m_actions.arraySize() <= 0)
            return false;
        return true;
    }

private:
    bool checkAction(const string& action);
    bool checkUri(const Uri& uri);
    bool checkMime(const string& mime);

    const static string CLASS_NAME;

    string m_id;
    string m_type;  // "appinfo", "runtime"
    int m_priority;

    JValue m_actions;
    JValue m_mimeTypes;
    JValue m_uris;
};

typedef shared_ptr<Handler> HandlerPtr;

#endif /* BASE_HANDLER_H_ */
