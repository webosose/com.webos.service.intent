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

#include "util/Logger.h"
#include "util/JValueUtil.h"

const string Handler::CLASS_NAME = "Handler";

Handler::Handler()
    : m_id(""),
      m_type(""),
      m_priority(0)
{
    m_actions = pbnjson::Array();
    m_mimeTypes = pbnjson::Array();
    m_uris = pbnjson::Array();
}

Handler::~Handler()
{
}

bool Handler::launch(Intent intent)
{
    return true;
}

bool Handler::isMatched(IntentPtr intent)
{
    if (checkAction(intent->getAction()) == false ||
        checkUri(intent->getUri()) == false ||
        checkMime(intent->getMimeType()) == false)
        return false;
    return true;
}

bool Handler::fromJson(const JValue& json)
{
    JValueUtil::getValue(json, "id", m_id);
    JValueUtil::getValue(json, "type", m_type);
    JValueUtil::getValue(json, "priority", m_priority);

    JValueUtil::getValue(json, "actions", m_actions);
    if (!m_actions.isArray()) {
        Logger::warning(CLASS_NAME, __FUNCTION__, "'actions' is not array");
        m_actions = pbnjson::Array();
    }
    JValueUtil::getValue(json, "mimeTypes", m_mimeTypes);
    if (!m_mimeTypes.isArray()) {
        Logger::warning(CLASS_NAME, __FUNCTION__, "'mimeTypes' is not array");
        m_mimeTypes = pbnjson::Array();
    }
    JValueUtil::getValue(json, "uris", m_uris);
    if (!m_uris.isArray()) {
        Logger::warning(CLASS_NAME, __FUNCTION__, "'uris' is not array");
        m_uris = pbnjson::Array();
    }
    return true;
}

bool Handler::toJson(JValue& json)
{
    json.put("id", m_id);
    json.put("type", m_type);
    json.put("priority", m_priority);

    json.put("actions", m_actions);
    json.put("mimeTypes", m_mimeTypes);
    json.put("uris", m_uris);

    return true;
}

bool Handler::checkAction(const string& action)
{
    if (action.empty())
        return true;

    for (JValue item : m_actions.items()) {
        if (item.asString() == action) {
            return true;
        }
    }
    return false;
}

bool Handler::checkUri(const Uri& uri)
{
    if (uri.empty())
        return true;

    Uri uriObj;
    for (JValue item : m_uris.items()) {
        if (!uriObj.parse(item.asString())) continue;
        if (uriObj == uri)
            return true;
    }
    return false;
}

bool Handler::checkMime(const string& mime)
{
    if (mime.empty())
        return true;

    for (JValue item : m_mimeTypes.items()) {
        if (item.asString() == mime) {
            return true;
        }
    }
    return false;
}
