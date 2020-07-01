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

#include "Session.h"

#include "base/Handlers.h"

const string& Session::CLASS_NAME = "Session";

Session::Session(const string& sessionId, GMainLoop* mainloop)
    : m_sessionId(sessionId),
      m_sam(sessionId),
      m_isRunning(true)
{
    Logger::info(CLASS_NAME, __FUNCTION__, m_sessionId, "Session is added");
    m_sam.initialize(mainloop);
}

Session::~Session()
{
    m_sam.finalize();
    Handlers::getInstance().removeBySessionId(m_sessionId);
    Logger::info(CLASS_NAME, __FUNCTION__, m_sessionId, "Session is removed");
}

