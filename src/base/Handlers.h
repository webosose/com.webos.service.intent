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

#ifndef BASE_HANDLERS_H_
#define BASE_HANDLERS_H_

#include <iostream>
#include <map>

#include "Handler.h"
#include "interface/IClassName.h"
#include "interface/ISingleton.h"
#include "interface/ISerializable.h"

using namespace std;

class Handlers : public IClassName,
                 public ISingleton<Handlers>,
                 public ISerializable {
friend class ISingleton<Handlers>;
public:
    virtual ~Handlers();

    virtual bool add(HandlerPtr handler);
    virtual bool removeBySessionId(const string& sessionId);
    virtual bool hasHandler(const string& key);
    virtual HandlerPtr getHandler(IntentPtr intent);

    // ISerializable
    virtual bool toJson(JValue& array) override;
    virtual bool toJson(JValue& array, const string& sessionId);
    virtual bool toJson(JValue& array, IntentPtr intent);

private:
    Handlers();

    map<string, HandlerPtr> m_handlers;

};

#endif /* BASE_HANDLERS_H_ */
