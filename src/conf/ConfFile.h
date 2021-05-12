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

#ifndef CONF_CONFFILE_H_
#define CONF_CONFFILE_H_

#include <iostream>
#include <pbnjson.hpp>

#include "interface/IInitializable.h"
#include "interface/ISingleton.h"

using namespace std;
using namespace pbnjson;

class ConfFile : public ISingleton<ConfFile>,
                 public IInitializable {
friend class ISingleton<ConfFile>;
public:
    virtual ~ConfFile();

    virtual bool getIntentFilter(const string& appId, JValue& intentFilter);

protected:
    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

private:
    ConfFile();

    static const string PATH_DATABASE;
    static const string PATH_TMP_DATABASE;

    JValue m_database;
};

#endif /* CONF_CONFFILE_H_ */
