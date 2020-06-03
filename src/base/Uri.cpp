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

#include "Uri.h"

Uri::Uri()
    : m_uri(""),
      m_scheme(""),
      m_host(""),
      m_path(""),
      m_query(""),
      m_fragment(""),
      m_empty(true)
{

}

Uri::Uri(const string& str)
    : Uri()
{
    parse(str);
}

Uri::~Uri()
{
}

bool Uri::parse(const string& str)
{
    static const std::regex URL(R"(^(([^:\/?#]+):)?(//([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)", std::regex::extended);

    // clear
    m_empty = true;
    m_uri = str;
    m_scheme = "";
    m_host = "";
    m_path = "";
    m_query = "";
    m_fragment = "";

    // See : https://stackoverflow.com/questions/5620235/cpp-regular-expression-to-validate-url/31613265#31613265
    std::smatch result;
    if (std::regex_match(m_uri, result, URL)) {
        unsigned counter = 0;
        for (const auto &res : result) {
            switch (counter) {
            case 2: m_scheme = res;   break;
            case 4: m_host = res;     break;
            case 5: m_path = res;     break;
            case 7: m_query = res;    break;
            case 9: m_fragment = res; break;
            default:
                break;
            }
            counter++;
        }
        // For debugging purpose.
        printf("scheme(%s) host(%s) path(%s) query(%s) fragment(%s)\n", m_scheme.c_str(), m_host.c_str(), m_path.c_str(), m_query.c_str(), m_fragment.c_str());

        m_empty = false;
    }
    return !m_empty;
}

bool Uri::operator==(const Uri& another)
{
    if (!m_scheme.empty() && m_scheme != another.m_scheme) return false;
    if (!m_host.empty() && m_host != another.m_host) return false;
    if (!m_path.empty() && m_path != another.m_path) return false;
    return true;
}
