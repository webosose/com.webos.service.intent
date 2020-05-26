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

#include <base/Intent.h>
#include <base/Uri.h>
#include <iostream>
#include <deque>
#include <map>

#include "interface/ISerializable.h"

using namespace std;

enum HandlerType {
    HandlerType_Unknown,
    HandlerType_AppInfo,
    HandlerType_Runtime,
};

class Handler : public ISerializable {
friend class Handlers;
public:
    static string toString(enum HandlerType type);
    static enum HandlerType toEnum(string type);

    Handler();
    virtual ~Handler();

    virtual bool launch(Intent intent);
    virtual bool isMatched(const Intent& intent);

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

    const int getPriority()
    {
        return m_priority;
    }

    const deque<string>& getActions()
    {
        return m_actions;
    }

    const deque<string>& getMimeTypes()
    {
        return m_mimeTypes;
    }

    const deque<Uri>& getUris()
    {
        return m_uris;
    }

    void setType(HandlerType type)
    {
        m_type = type;
    }

    const HandlerType getType()
    {
        return m_type;
    }

    const bool isValid()
    {
        if (m_id.empty() || m_actions.size() == 0 || m_type == HandlerType_Unknown)
            return false;
        return true;
    }

private:
    static void getUniqueArray(const JValue& from, JValue& to);

    string m_id;
    int m_priority;

    deque<string> m_actions;
    deque<string> m_mimeTypes;
    deque<Uri> m_uris;

    enum HandlerType m_type;
};

typedef shared_ptr<Handler> HandlerPtr;

#endif /* BASE_HANDLER_H_ */
