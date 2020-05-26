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
#include <regex>

#include "interface/ISerializable.h"

using namespace std;

class Uri {
public:
    Uri();
    Uri(const string& str);
    virtual ~Uri();

    bool parse(const string& str);

    string uri()      const { return m_uri; }
    string scheme()   const { return m_scheme; }
    string host()     const { return m_host; }
    string path()     const { return m_path; }
    string query()    const { return m_query; }
    string fragment() const { return m_fragment; }

    string toString() const { return m_uri; }
    bool isValid()    const { return m_isValid; }

private:
    string m_uri;
    string m_scheme;
    string m_host;
    string m_path;
    string m_query;
    string m_fragment;

    bool m_isValid;
};

#endif /* BASE_URI_H_ */
