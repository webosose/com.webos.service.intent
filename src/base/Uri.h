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

class Uri {
public:
    Uri();
    Uri(const string& str);
    virtual ~Uri();

    bool isValid();

    string scheme()   const { return fromRange(m_uri.scheme); }
    string host()     const { return fromRange(m_uri.hostText); }
    string port()     const { return fromRange(m_uri.portText); }
    string path()     const { return fromList(m_uri.pathHead, "/"); }
    string query()    const { return fromRange(m_uri.query); }
    string fragment() const { return fromRange(m_uri.fragment); }

    const string toString() const;
    virtual bool fromString(const string& str);

private:
    string fromRange(const UriTextRangeA & range) const
    {
        return std::string(range.first, range.afterLast);
    }
    std::string fromList(UriPathSegmentA * xs, const std::string & delim) const
    {
        UriPathSegmentStructA * head(xs);
        std::string accum;

        while (head)
        {
            accum += delim + fromRange(head->text);
            head = head->next;
        }

        return accum;
    }

    UriUriA m_uri;
    bool m_isValid;
};

#endif /* BASE_URI_H_ */
