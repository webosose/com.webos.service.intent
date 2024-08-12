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

#include "Intents.h"

#include "util/Logger.h"

Intents::Intents()
{
    setClassName("Intents");
}

Intents::~Intents()
{
}

void Intents::add(IntentPtr intent)
{
    m_intents[intent->getIntentId()] = intent;
    onAdd(std::move(intent));
}

void Intents::removeById(int intentId)
{
    IntentPtr intent = getById(intentId);
    if (intent == nullptr)
        return;
    onRemove(std::move(intent));
    m_intents.erase(intentId);
}

void Intents::removeByOwner(const string& owner)
{
    for (auto it = m_intents.cbegin(); it != m_intents.cend();) {
        if (it->second->getOwner() == owner) {
            onRemove(it->second);
            it = m_intents.erase(it);
        } else {
            ++it;
        }
    }
}

IntentPtr Intents::getById(int intentId)
{
    if (m_intents.find(intentId) == m_intents.end()) {
        return nullptr;
    }
    return m_intents[intentId];
}

IntentPtr Intents::getByToken(int token)
{
    for (auto it = m_intents.begin(); it != m_intents.end(); ++it) {
        if (it->second->getToken() == token) {
            return it->second;
        }
    }
    return nullptr;
}

void Intents::onRemove(IntentPtr intent)
{
    Logger::info(getClassName(), __FUNCTION__, toString(std::move(intent)), "Removed");
}

void Intents::onAdd(IntentPtr intent)
{
    Logger::info(getClassName(), __FUNCTION__, toString(std::move(intent)), "Added");
}
