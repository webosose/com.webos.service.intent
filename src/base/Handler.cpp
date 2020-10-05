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
    : m_name("")
{
    m_intentFilters = pbnjson::Array();
}

Handler::~Handler()
{
}

bool Handler::isMatched(IntentPtr intent)
{
    if (intent->hasSessionId() && m_sessionId != intent->getSessionId()) {
        return false;
    }

    // explicit intent
    if (intent->hasName()) {
        if (m_name == intent->getName())
            return true;
        return false;
    }

    // implicit intent
    for (JValue intentFilter : m_intentFilters.items()) {
        if (compareIntentFilter(intentFilter, intent))
            return true;
    }
    return false;
}

bool Handler::toJson(JValue& json)
{
    json.put("name", m_name);
    if (!m_sessionId.empty())
        json.put("sessionId", m_sessionId);
    json.put("intentFilters", m_intentFilters);
    return true;
}

bool Handler::compareIntentFilter(JValue& intentFilter, IntentPtr intent)
{
    JValue actions;
    if (intent->hasAction()) { // This might be always true because 'empty' action is not allowed.
        if (!JValueUtil::getValue(intentFilter, "actions", actions) || !actions.isArray()) {
            return false;
        }
        if (!compareString(actions, intent->getAction())) {
            return false;
        }
    }

    JValue uris;
    JValue mimeTypes;
    if (intent->hasUri() && intent->hasMimeType()) {
        if (!JValueUtil::getValue(intentFilter, "uris", uris) || !uris.isArray()) {
            return false;
        }
        if (!JValueUtil::getValue(intentFilter, "mimeTypes", mimeTypes) || !mimeTypes.isArray()) {
            return false;
        }
        if (!compareUri(uris, intent->getUri())) {
            return false;
        }
        if (!compareString(mimeTypes, intent->getMimeType())) {
            return false;
        }
    } else if (intent->hasUri() && !intent->hasMimeType()) {
        if (intentFilter.hasKey("mimeTypes")) {
            return false;
        }
        if (!JValueUtil::getValue(intentFilter, "uris", uris) || !uris.isArray()) {
            return false;
        }
        if (!compareUri(uris, intent->getUri())) {
            return false;
        }
    } else if (!intent->hasUri() && intent->hasMimeType()) {
        if (intentFilter.hasKey("uris")) {
            return false;
        }
        if (!JValueUtil::getValue(intentFilter, "mimeTypes", mimeTypes) || !mimeTypes.isArray()) {
            return false;
        }
        if (!compareString(mimeTypes, intent->getMimeType())) {
            return false;
        }
    } else {
        if (intentFilter.hasKey("uris") || intentFilter.hasKey("mimeTypes")) {
            return false;
        }
    }
    return true;
}

bool Handler::compareString(JValue& array, const string& target)
{
    for (JValue item : array.items()) {
        if (item.asString() == target) {
            return true;
        }
    }
    return false;
}

bool Handler::compareUri(JValue& uris, const Uri& b)
{
    if (b.empty())
        return true;

    Uri a;
    for (JValue item : uris.items()) {
        if (a.parse(item.asString()) == false) {
            continue;
        }
        if (Uri::compare(a, b)) {
            return true;
        }
    }
    return false;
}
