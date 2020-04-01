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

#include <base/URI.h>

URI::URI()
{
}

URI::URI(const string& str)
{

}

URI::~URI()
{
}

bool URI::empty()
{
    return true;
}

string& URI::scheme()
{
    return m_str;
}

string& URI::path()
{
    return m_str;
}

const string& URI::toString() const
{
    return m_str;
}

bool URI::fromString(const string& str)
{
    return true;
}

bool URI::fromJson(const JValue& json)
{
    return true;
}

bool URI::toJson(JValue& json)
{
    return true;
}
