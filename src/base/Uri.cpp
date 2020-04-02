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

#include <base/Uri.h>

Uri::Uri()
    : m_isValid(false)
{
}

Uri::Uri(const string& str)
    : m_isValid(false)
{
    fromString(str);
}

Uri::~Uri()
{
    if (m_isValid) {
        uriFreeUriMembersA(&m_uri);
    }
}

bool Uri::isValid()
{
    return m_isValid;
}

const string Uri::toString() const
{
    int charsRequired = -1;
    if (uriToStringCharsRequiredA(&m_uri, &charsRequired) != URI_SUCCESS) {
        return "";
    }
    charsRequired++;

    char* uriString = (char*)malloc(charsRequired * sizeof(char));
    if (uriString == NULL) {
        return "";
    }
    if (uriToStringA(uriString, &m_uri, charsRequired, NULL) != URI_SUCCESS) {
        return "";
    }
    return uriString;
}

bool Uri::fromString(const string& str)
{
    if (m_isValid) {
        uriFreeUriMembersA(&m_uri);
        m_isValid = false;
    }
    const char * errorPos;
    if (uriParseSingleUriA(&m_uri, str.c_str(), &errorPos) != URI_SUCCESS) {
        m_isValid = false;
    } else {
        m_isValid = true;
    }
    return m_isValid;
}

