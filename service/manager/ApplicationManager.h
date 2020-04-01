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

#ifndef MANAGER_APPLICATIONMANAGER_H_
#define MANAGER_APPLICATIONMANAGER_H_

#include <base/Handler.h>
#include <iostream>

#include <luna-service2/lunaservice.hpp>

#include "interface/IManageable.h"

using namespace std;

class ApplicationManager : public IManageable<ApplicationManager> {
friend class IManageable<ApplicationManager>;
public:
    virtual ~ApplicationManager();

    virtual bool onInitialization() override;
    virtual bool onFinalization() override;
    static bool onServerStatus(bool isConnected);

    bool launch(Intent& intent, Handler& handler);

private:
    ApplicationManager();

    static bool _listApps(LSHandle* sh, LSMessage* reply, void* ctx);
    void listApps();

    LS::ServerStatus m_serverStatus;
    LS::Call m_listApps;
};

#endif /* MANAGER_APPLICATIONMANAGER_H_ */
