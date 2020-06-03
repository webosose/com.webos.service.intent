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

#include "Intent.h"

#include "util/JValueUtil.h"
#include "util/Logger.h"

const string Intent::CLASS_NAME = "Intent";

Intent::Intent()
    : m_requester(""),
      m_action(""),
      m_mimeType(""),
      m_result(""),
      m_uri(""),
      m_chooser(false),
      m_uriObj()

{
    m_extra = pbnjson::Object();
}

Intent::~Intent()
{

}

bool Intent::fromJson(const JValue& json)
{
    JValueUtil::getValue(json, "requester", m_requester);
    JValueUtil::getValue(json, "action", m_action);
    JValueUtil::getValue(json, "mimeType", m_mimeType);
    JValueUtil::getValue(json, "result", m_result);
    JValueUtil::getValue(json, "extra", m_extra);
    JValueUtil::getValue(json, "chooser", m_chooser);
    if (JValueUtil::getValue(json, "uri", m_uri)) {
        m_uriObj.parse(m_uri);
    }
    return true;
}

bool Intent::toJson(JValue& json)
{
    json.put("requester", m_requester);
    json.put("action", m_action);
    json.put("uri", m_uriObj.toString());
    json.put("mimeType", m_mimeType);
    json.put("result", m_result);
    json.put("extra", m_extra.duplicate());
    return true;
}
