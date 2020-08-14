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

#include "core/IntentManager.h"
#include "util/JValueUtil.h"
#include "util/Logger.h"

const string Intent::CLASS_NAME = "Intent";

Intent::Intent()
    : m_intentId(-1)
{
    m_subscriptionPoint.setServiceHandle(&IntentManager::getInstance());
    m_extra = pbnjson::Object();
}

Intent::~Intent()
{

}

bool Intent::fromJson(const JValue& json)
{
    if (json.isNull() || !json.isValid())
        return false;

    JValueUtil::getValue(json, "intentId", m_intentId);
    JValueUtil::getValue(json, "name", m_name);
    JValueUtil::getValue(json, "action", m_action);
    JValueUtil::getValue(json, "mimeType", m_mimeType);
    JValueUtil::getValue(json, "result", m_result);
    JValueUtil::getValue(json, "extra", m_extra);
    if (json.hasKey("uri")) {
        m_uri.parse(json["uri"].asString());
    }
    return true;
}

bool Intent::toJson(JValue& json)
{
    if (json.isNull() || !json.isValid())
        json = pbnjson::Object();

    if (m_intentId != -1)
        json.put("intentId", m_intentId);
    if (!m_name.empty())
        json.put("name", m_name);
    if (!m_action.empty())
        json.put("action", m_action);
    if (!m_mimeType.empty())
        json.put("mimeType", m_mimeType);
    if (!m_result.empty())
        json.put("result", m_result);
    if (m_extra.objectSize() != 0)
        json.put("extra", m_extra.duplicate());

    if (!m_uri.empty()) {
        JValue parsedUri = pbnjson::Object();
        m_uri.toJson(parsedUri);
        json.put("parsedUri", parsedUri);
        json.put("uri", m_uri.toString());
    }
    return true;
}
