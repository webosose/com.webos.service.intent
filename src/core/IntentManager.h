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

#ifndef CORE_INTENTMANAGER_H_
#define CORE_INTENTMANAGER_H_

#include <iostream>
#include <luna-service2/lunaservice.hpp>
#include <pbnjson.hpp>
#include <queue>

#include "interface/IInitializable.h"
#include "interface/ISingleton.h"

using namespace std;
using namespace LS;
using namespace pbnjson;

class IntentManager : public Handle,
                      public ISingleton<IntentManager>,
                      public IInitializable {
friend class ISingleton<IntentManager>;

public:
    virtual ~IntentManager();

    // APIs
    bool query(LSMessage& message);
    bool start(LSMessage& message);
    bool sendResult(LSMessage& message);
    bool subscribeResult(LSMessage& message);

    void subscribeStatus(const string& name, const string& sessionId);

protected:
    // IManageable
    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

private:
    static bool _onServerStatusChanged(LSHandle* sh, LSMessage* message, void* context);
    static string getName(LS::Message& request);

    IntentManager();
};

#endif /* CORE_INTENTMANAGER_H_ */
