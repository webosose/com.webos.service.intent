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

#ifndef INTERFACE_IINITIALIZABLE_H_
#define INTERFACE_IINITIALIZABLE_H_

#include <glib.h>

#include <iostream>

#include "IClassName.h"
#include "util/Logger.h"

using namespace std;

class IInitializable : public IClassName {
public:
    virtual ~IInitializable(){};

    virtual bool initialize(GMainLoop* mainloop) final
    {
        Logger::info(getClassName(), __FUNCTION__, "Start initialization");
        m_mainloop = mainloop;
        m_isInitalized = onInitialization();
        Logger::info(getClassName(), __FUNCTION__, "End initialization");
        return m_isInitalized;
    }

    virtual bool finalize() final
    {
        Logger::info(getClassName(), __FUNCTION__, "Start finalization");
        m_isFinalized = onFinalization();
        Logger::info(getClassName(), __FUNCTION__, "End finalization");
        return m_isFinalized;
    }

    virtual bool isInitalized()
    {
        return m_isInitalized;
    }

    virtual bool isFinalized()
    {
        return m_isFinalized;
    }

    virtual bool onInitialization() = 0;
    virtual bool onFinalization() = 0;

protected:
    IInitializable()
        : m_mainloop(nullptr),
          m_isInitalized(false),
          m_isFinalized(false){};

    GMainLoop* m_mainloop;

private:
    bool m_isInitalized;
    bool m_isFinalized;
};

#endif /* INTERFACE_IINITIALIZABLE_H_ */
