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

#ifndef CONF_CONFIGFILE_H_
#define CONF_CONFIGFILE_H_

#include <iostream>

#include <pbnjson.hpp>

#include "interface/ISingleton.h"
#include "interface/IInitializable.h"

using namespace std;
using namespace pbnjson;

class ConfigFile : public ISingleton<ConfigFile>,
                   public IInitializable {
friend class ISingleton<ConfigFile>;
public:
    virtual ~ConfigFile();

    virtual JValue& getDebugDB();
    virtual JValue& getRuntimeDB();
    virtual void syncRuntime();

    bool isTest()
    {
        return m_isTest;
    }

    bool isVerbose()
    {
        return m_isVerbose;
    }

protected:
    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

private:
    static const string DEBUG_DB_PATH;
    static const string RUNTIME_DB_PATH;

    ConfigFile();

    JValue m_debugDB;
    JValue m_runtimeDB;

    bool m_isTest;
    bool m_isVerbose;
};

#endif /* CONF_CONFIGFILE_H_ */
