// Copyright (c) 2020-2021 LG Electronics, Inc.
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

#include "Handlers.h"

#include "conf/ConfFile.h"
#include "util/JValueUtil.h"

Handlers::Handlers()
{
    setClassName("Handlers");
}

Handlers::~Handlers()
{
}

bool Handlers::add(HandlerPtr handler)
{
    if (handler == nullptr || !handler->hasName()) {
        Logger::warning(getClassName(), __FUNCTION__, "Invalid handler");
        return false;
    }

    if (hasHandler(handler->getKey())) {
        Logger::warning(getClassName(), __FUNCTION__, "The id is already existed");
        return false;
    }
    m_handlers[handler->getKey()] = handler;
    return true;
}

bool Handlers::remove(const string& key)
{
    m_handlers.erase(key);
    return true;
}

bool Handlers::removeBySessionId(const string& sessionId)
{
    for (auto it = m_handlers.cbegin(); it != m_handlers.cend();) {
        if (it->second->getSessionId() == sessionId) {
            it = m_handlers.erase(it);
        } else {
            ++it;
        }
    }
    return true;
}

bool Handlers::hasHandler(const string& key)
{
    if (m_handlers.find(key) == m_handlers.end())
        return false;
    return true;
}

HandlerPtr Handlers::getHandler(IntentPtr intent)
{
    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if (it->second->isMatched(intent)) {
            return it->second;
        }
    }
    return nullptr;
}

bool Handlers::toJson(JValue& array)
{
    return toJson(array, "");
}

bool Handlers::toJson(JValue& array, const string& sessionId)
{
    if (!array.isArray()) {
        array = pbnjson::Array();
    }

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        JValue item = pbnjson::Object();

        if (!sessionId.empty() && sessionId != it->second->getSessionId()) {
            // skip
        } else {
            it->second->toJson(item);
            array.append(item);
        }
    }
    return true;
}

bool Handlers::toJson(JValue& array, IntentPtr intent)
{
    if (!array.isArray()) {
        array = pbnjson::Array();
    }

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if (!it->second->isMatched(intent)) {
            continue;
        }

        JValue item = pbnjson::Object();
        it->second->toJson(item);
        array.append(item);
    }
    return true;
}
