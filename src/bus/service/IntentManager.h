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

#ifndef BUS_SERVICE_INTENTMANAGER_H_
#define BUS_SERVICE_INTENTMANAGER_H_

#include <iostream>

#include "base/Handler.h"
#include "base/Handlers.h"
#include "base/Intent.h"
#include "bus/client/SAM.h"
#include "bus/service/LS2Handler.h"
#include "conf/ConfigFile.h"
#include "interface/ISingleton.h"
#include "interface/IInitializable.h"
#include "util/Logger.h"

using namespace std;

class IntentManager : public ISingleton<IntentManager>,
                      public IInitializable {
friend class ISingleton<IntentManager>;
public:
    virtual ~IntentManager();

    virtual bool launch(Intent& intent);
    virtual JValue resolve(Intent& intent);
    virtual JValue getAllHandlers();
    virtual JValue getHandler(const string& id);

protected:
    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

private:
    IntentManager();

    virtual void loadConfig(const JValue& json);

    static Intent CHOOSER;

};

#endif /* BUS_SERVICE_INTENTMANAGER_H_ */
