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

#include <base/Intent.h>
#include "util/Logger.h"

Intent::Intent()
    : m_requester("")
    , m_action("")
    , m_mimeType("")
    , m_result("")
    , m_uri()
    , m_chooser(false)
{
    setClassName("Intent");
    m_extra = pbnjson::Object();
}

Intent::~Intent()
{

}

bool Intent::fromJson(const JValue& json)
{
    if (json.hasKey("id") && json["id"].isString()) {
        m_id = json["id"].asString();
    }
    if (json.hasKey("requester") && json["requester"].isString()) {
        m_requester = json["requester"].asString();
    }
    if (json.hasKey("action") && json["action"].isString()) {
        m_action = json["action"].asString();
    }
    if (json.hasKey("mimeType") && json["mimeType"].isString()) {
        m_mimeType = json["mimeType"].asString();
    }
    if (json.hasKey("result") && json["result"].isString()) {
        m_result = json["result"].asString();
    }
    if (json.hasKey("extra") && json["extra"].isObject()) {
        m_extra = json["extra"].duplicate();
    }
    if (json.hasKey("chooser") && json["chooser"].isBoolean()) {
        m_chooser = json["chooser"].asBool();
    }
    if (json.hasKey("uri") && json["uri"].isString()) {
        m_uri.fromString(json["uri"].asString());
    }
    return true;
}

bool Intent::toJson(JValue& json)
{
    json.put("id", m_id);
    json.put("requester", m_requester);
    json.put("action", m_action);
    json.put("uri", m_uri.toString());
    json.put("mimeType", m_mimeType);
    json.put("result", m_result);
    json.put("extra", m_extra.duplicate());
    return true;
}
