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

#include "MainDaemon.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory>
#include <glib.h>
#include <boost/bind.hpp>

#include "base/Handlers.h"
#include "bus/client/SAM.h"
#include "bus/service/IntentManager.h"
#include "conf/ConfFile.h"
#include "util/File.h"
#include "util/JValueUtil.h"


MainDaemon::MainDaemon()
{
    setClassName("MainDaemon");
    m_mainLoop = g_main_loop_new(NULL, FALSE);
}

MainDaemon::~MainDaemon()
{
    if (m_mainLoop) {
        g_main_loop_unref(m_mainLoop);
    }
}

void MainDaemon::initialize()
{
    ConfFile::getInstance().initialize(m_mainLoop);
    Handlers::getInstance().load();

    IntentManager::getInstance().initialize(m_mainLoop);
    SAM::getInstance().initialize(m_mainLoop);

    if (!ConfFile::getInstance().testmode())
        return;

    JValue& database = ConfFile::getInstance().get();
    if (!database.hasKey("steps") || !database["steps"].isArray())
        return;

    for (JValue step : database["steps"].items()) {
        string type;
        JValue payload;

        if (JValueUtil::getValue(step, "type", type) && JValueUtil::getValue(step, "payload", payload)) {
            if (type == "listApps") {
                SAM::getInstance().listApps(payload);
            }
        }
    }
}

void MainDaemon::finalize()
{
    SAM::getInstance().finalize();
    IntentManager::getInstance().finalize();

    Handlers::getInstance().save();
    ConfFile::getInstance().finalize();
}

void MainDaemon::start()
{
    Logger::info(getClassName(), __FUNCTION__, "Start event handler thread");
    g_main_loop_run(m_mainLoop);
}

void MainDaemon::stop()
{
    if (m_mainLoop)
        g_main_loop_quit(m_mainLoop);
}

