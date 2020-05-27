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

#include "Handlers.h"

#include "conf/ConfigFile.h"

Handlers::Handlers()
{
    setClassName("Handlers");
    m_chooser.setAction("action_chooser");
}

Handlers::~Handlers()
{
}

void Handlers::save()
{
    JValue handlers = pbnjson::Array();
    toJson(handlers, HandlerType_Runtime);

    if (ConfigFile::getInstance().getRuntimeDB()["handlers"] != handlers) {
        ConfigFile::getInstance().getRuntimeDB().put("handlers", handlers);
        Logger::info(getClassName(), __FUNCTION__, "Save changed runtime handlers");
    }
}

void Handlers::load()
{
    JValue handlers = pbnjson::Array();
    for (JValue item : handlers.items()) {
        HandlerPtr handler = make_shared<Handler>();
        handler->fromJson(item);
        Handlers::getInstance().add(handler, HandlerType_AppInfo);
    }
}

bool Handlers::add(HandlerPtr handler, enum HandlerType type)
{
    if (handler == nullptr || handler->getType() != type) {
        return false;
    }

    if (getHandler(handler->getId()) != nullptr) {
        // already existing. Please use update API instead
        return false;
    }

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if ((*it)->getPriority() < handler->getPriority()) {
            m_handlers.insert(it, handler);
            return true;
        }
    }
    m_handlers.push_back(handler);
    return true;
}

bool Handlers::remove(const string& id, enum HandlerType type)
{
    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if ((*it)->getId() == id && (*it)->getType() == type) {
            m_handlers.erase(it);
            return true;
        }
    }
    return false;
}

bool Handlers::hasHandler(const string& id)
{
    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if ((*it)->getId() == id) {
            return true;
        }
    }
    return false;
}

HandlerPtr Handlers::getChooser()
{
    return getHandler(m_chooser);
}

HandlerPtr Handlers::getHandler(const string& id)
{
    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if ((*it)->getId() == id) {
            return (*it);
        }
    }
    return nullptr;
}

HandlerPtr Handlers::getHandler(const Intent& intent)
{
    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if ((*it)->isMatched(intent)) {
            return (*it);
        }
    }
    return nullptr;
}

bool Handlers::toJson(JValue& array)
{
    if (!array.isArray()) {
        array = pbnjson::Array();
    }

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        JValue item = pbnjson::Object();
        (*it)->toJson(item);
        array.append(item);
    }
    return true;
}

bool Handlers::toJson(JValue& array, const enum HandlerType type)
{
    if (!array.isArray()) {
        array = pbnjson::Array();
    }

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if ((*it)->getType() != type) continue;
        JValue item = pbnjson::Object();
        (*it)->toJson(item);
        array.append(item);
    }
    return true;
}

bool Handlers::toJson(JValue& array, const Intent& intent)
{
    if (!array.isArray()) {
        array = pbnjson::Array();
    }

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if (!(*it)->isMatched(intent)) continue;
        JValue item = pbnjson::Object();
        (*it)->toJson(item);
        array.append(item);
    }
    return true;
}

bool Handlers::toJson(JValue& json, const string& id)
{
    if (!json.isObject()) {
        json = pbnjson::Object();
    }

    HandlerPtr handler = getHandler(id);
    if (handler == nullptr)
        return false;

    handler->toJson(json);
    return true;
}
