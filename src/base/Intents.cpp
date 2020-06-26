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

#include "Intents.h"

Intents::Intents()
{
}

Intents::~Intents()
{
}

void Intents::add(IntentPtr intent)
{
    m_intents[intent->getIntentId()] = intent;
}

void Intents::remove(int intentId)
{
    m_intents.erase(intentId);
}

void Intents::removeByOwner(const string& owner)
{

}

IntentPtr Intents::get(int intentId)
{
    if (m_intents.find(intentId) == m_intents.end())
        return nullptr;
    return m_intents[intentId];
}
