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

#include <iostream>
#include <glib.h>
#include <manager/ConfigManager.h>

#include "manager/ApplicationManager.h"
#include "manager/HandlerManager.h"
#include "manager/IntentManager.h"

using namespace std;

static GMainLoop *s_mainloop;

void exitDaemon(int signo)
{
    g_main_loop_quit(s_mainloop);
}

int main()
{
    signal(SIGTERM, exitDaemon);
    signal(SIGINT, exitDaemon);

    s_mainloop = g_main_loop_new(NULL, FALSE);

    // xxx: DON'T change initialization order.
    ConfigManager::getInstance().initialize(s_mainloop);
    HandlerManager::getInstance().initialize(s_mainloop);
    IntentManager::getInstance().initialize(s_mainloop);
    ApplicationManager::getInstance().initialize(s_mainloop);

    // waiting all manager ready
    g_main_context_iteration(NULL, true);

    // start LS2 API handling
    g_main_loop_run(s_mainloop);

    // xxx: DON'T change finalize order.
    ApplicationManager::getInstance().finalize();
    IntentManager::getInstance().finalize();
    HandlerManager::getInstance().finalize();
    ConfigManager::getInstance().finalize();

    g_main_loop_unref(s_mainloop);

    return 0;
}
