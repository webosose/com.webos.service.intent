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

#include "Handler.h"

#include <algorithm>

string Handler::toString(enum HandlerType type)
{
    switch(type) {
    case HandlerType_AppInfo:
        return "appinfo";

    case HandlerType_Runtime:
        return "runtime";

    default:
        return "unknown";
    }
    return "unknown";
}

enum HandlerType Handler::toEnum(string type)
{
    if (type == "appinfo") {
        return HandlerType_AppInfo;
    } else if (type == "runtime") {
        return HandlerType_Runtime;
    } else {
        return HandlerType_Unknown;
    }
}

Handler::Handler()
    : m_id("")
    , m_priority(0)
    , m_type(HandlerType_Unknown)
{
}

Handler::~Handler()
{
}

bool Handler::launch(Intent intent)
{
    return true;
}

bool Handler::isMatched(const Intent& intent)
{
    if (!intent.checkAction()) {
        return false;
    }

    // If 'id' is there, only 'id' is the key parameter
    if (!intent.m_id.empty()) {
        if (m_id == intent.m_id) {
            return true;
        }
        else {
            return false;
        }
    }

    if (getId() == intent.m_id)
        return true;

    // check 'ACTION'
    deque<string>::iterator i;
    i = find_if(m_actions.begin(), m_actions.end(),
                [&] (const string& action) { return action == intent.m_action; } );
    if (i == m_actions.end()) {
        return false;
    }

    // check URI
    if (!m_uris.empty()) {
//        string_view schema = intent.getUri().scheme();
//        string_view path = intent.getUri().path();
//        deque<URI>::iterator j;
//        j = find_if(m_uris.begin(), m_uris.end(),
//                    [&] (const URI& u)
//                    {
//                        cout << u.toString() << endl;
//                        if (u.scheme().compare(schema)) return false;
//                        cout << u.path() << endl;
//                        cout << path << endl;
//                        if (u.path().compare(path)) return false;
//                        return true;
//                    });
//        if (j == m_uris.end()) {
//            return false;
//        }
    }

    // check mimeType
    if (!m_mimeTypes.empty()) {
        deque<string>::iterator k;
        k = find_if(m_mimeTypes.begin(), m_mimeTypes.end(),
                     [&] (const string& mimeType) { return mimeType == intent.m_mimeType; } );
        if (k == m_mimeTypes.end()) {
            return false;
        }
    }
    return true;
}

bool Handler::fromJson(const JValue& json)
{
    if (json.hasKey("id") && json["id"].isString()) {
        m_id = json["id"].asString();
    }

    if (json.hasKey("priority") && json["priority"].isNumber()) {
        m_priority = json["priority"].asNumber<int>();
    }

    if (json.hasKey("type") && json["type"].isString()) {
        m_type = toEnum(json["type"].asString());
    }

    JValue array = pbnjson::Array();
    getUniqueArray(json["actions"], array);
    if (array.arraySize() > 0) {
        m_actions.clear();
        for (JValue action : array.items()) {
            m_actions.push_back(action.asString());
        }
    }

    array = pbnjson::Array();
    getUniqueArray(json["mimeTypes"], array);
    if (array.arraySize() > 0) {
        m_mimeTypes.clear();
        for (JValue mimeType : array.items()) {
            m_mimeTypes.push_back(mimeType.asString());
        }
    }

    array = pbnjson::Array();
    getUniqueArray(json["uris"], array);
    if (array.arraySize() > 0) {
        m_uris.clear();
        for (JValue uri : array.items()) {
            m_uris.push_back(URI(uri.asString()));
        }
    }
    return true;
}

bool Handler::toJson(JValue& json)
{
    json.put("id", m_id);
    json.put("priority", m_priority);
    json.put("type", toString(m_type));

    JValue actions = pbnjson::Array();
    for (unsigned int i = 0; i < m_actions.size(); ++i) {
        actions.append(m_actions[i]);
    }
    json.put("actions", actions);

    JValue mimeTypes = pbnjson::Array();
    for (unsigned int i = 0; i < m_mimeTypes.size(); ++i) {
        mimeTypes.append(m_mimeTypes[i]);
    }
    json.put("mimeTypes", mimeTypes);

    JValue uris = pbnjson::Array();
    for (unsigned int i = 0; i < m_uris.size(); ++i) {
        uris.append(m_uris[i].toString());
    }
    json.put("uris", uris);

    return true;
}

void Handler::getUniqueArray(const JValue& from, JValue& to)
{
    map<string, bool> checker;

    if (!from.isValid() || !from.isArray())
        return;
    for (JValue item : from.items()) {
        if (checker.find(item.asString()) == checker.end()) {
            checker[item.asString()] = true;
            to.append(item);
        }
    }
}
