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

#ifndef BASE_URI_H_
#define BASE_URI_H_

#include <iostream>
#include <uriparser/Uri.h>

#include "interface/ISerializable.h"

using namespace std;

class URI : public ISerializable {
public:
    URI();
    URI(const string& str);
    virtual ~URI();

    bool empty();

    string& scheme();
    string& path();
    const string& toString() const;

    virtual bool fromString(const string& str);
    virtual bool fromJson(const JValue& json);
    virtual bool toJson(JValue& json);

private:
    string m_str;
};

#endif /* BASE_URI_H_ */
