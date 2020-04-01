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

#ifndef MANAGER_HANDLERMANAGER_H_
#define MANAGER_HANDLERMANAGER_H_

#include <base/Handler.h>
#include <base/Intent.h>
#include <iostream>

#include "interface/IManageable.h"

using namespace std;

class HandlerManager : public IManageable<HandlerManager> {
friend class IManageable<HandlerManager>;
public:
    virtual ~HandlerManager();

    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

    virtual bool launch(Intent& intent);
    virtual JValue resolve(Intent& intent);
    virtual JValue getAllHandlers();
    virtual JValue getHandler(const string& id);
    virtual bool setHandler(Handler& handler);
    virtual bool registerHandler(Handler& handler);
    virtual bool unregisterHandler(Handler& handler);

private:
    HandlerManager();

    virtual void loadConfig(const JValue& json);

    virtual bool hasHandler(const string& id);
    virtual deque<Handler>::iterator findHandler(const string& id);

    static Intent CHOOSER;
    deque<Handler> m_handlers;

};

#endif /* MANAGER_HANDLERMANAGER_H_ */
