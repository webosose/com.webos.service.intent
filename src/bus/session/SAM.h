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

#ifndef BUS_SESSION_SAM_H_
#define BUS_SESSION_SAM_H_

#include <iostream>
#include <luna-service2/lunaservice.hpp>
#include <map>

#include "base/Handler.h"
#include "bus/AbsLunaClient.h"
#include "interface/IInitializable.h"
#include "interface/ISingleton.h"

using namespace std;

class SAM : public AbsLunaClient {
public:
    // SAM is session-side service.
    SAM(const string& sessionId);
    virtual ~SAM();

    // API
    int launch(IntentPtr intent, HandlerPtr handler);

protected:
    virtual void onServerStatusChanged(bool isConnected) override;

private:
    static bool onLaunch(LSHandle* sh, LSMessage* reply, void* ctx);
    static bool onListApps(LSHandle* sh, LSMessage* reply, void* ctx);

    static const string& CLASS_NAME;

    LSMessageToken m_listApps;
};

#endif /* BUS_SESSION_SAM_H_ */
