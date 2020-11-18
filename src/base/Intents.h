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

#ifndef BASE_INTENTS_H_
#define BASE_INTENTS_H_

#include <iostream>
#include <map>

#include "base/Intent.h"
#include "interface/IClassName.h"
#include "interface/ISingleton.h"

using namespace std;

class Intents : public IClassName,
                public ISingleton<Intents> {
friend class ISingleton<Intents>;
public:
    static string toString(IntentPtr intent)
    {
        static char buffer[1024];
        snprintf(buffer, 1024, "intentId(%d), owner(%s)", intent->getIntentId(), intent->getOwner().c_str());
        return string(buffer);
    }
    virtual ~Intents();

    void add(IntentPtr intent);
    void removeById(int intentId);
    void removeByOwner(const string& owner);

    IntentPtr getById(int intentId);
    IntentPtr getByToken(int token);

private:
    Intents();

    void onRemove(IntentPtr intent);
    void onAdd(IntentPtr intent);

    map<int, IntentPtr> m_intents;

};

#endif /* BASE_INTENTS_H_ */
