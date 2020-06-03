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

#include "base/Uri.h"
#include "interface/ISerializable.h"
#include "interface/IClassName.h"

using namespace std;

class Intent : public ISerializable {
public:
    Intent();
    virtual ~Intent();

    // ISerializable
    virtual bool fromJson(const JValue& json) override;
    virtual bool toJson(JValue& json) override;

    void setRequester(const string& requester)
    {
        m_requester = requester;
    }
    const string& getRequester()
    {
        return m_requester;
    }

    void setAction(const string& action)
    {
        m_action = action;
    }
    const string& getAction() const
    {
        return m_action;
    }

    const Uri& getUri() const
    {
        return m_uriObj;
    }

    const string& getMimeType() const
    {
        return m_mimeType;
    }

    const JValue& getExtra()
    {
        return m_extra;
    }

    const string& getResult()
    {
        return m_result;
    }

    const bool chooser()
    {
        return m_chooser;
    }

    virtual bool isValid() const
    {
        if (m_requester.empty() || m_action.empty() || m_uriObj.empty())
            return false;
        return true;
    }

private:
    const static string CLASS_NAME;

    string m_requester;
    string m_action;
    string m_mimeType;
    string m_result;
    string m_uri;
    JValue m_extra;
    bool m_chooser;

    Uri m_uriObj;
};

typedef shared_ptr<Intent> IntentPtr;

#endif /* BASE_INTENT_H_ */
