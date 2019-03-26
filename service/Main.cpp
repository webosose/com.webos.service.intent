/* @@@LICENSE
 *
 * Copyright (c) 2019 LG Electronics, Inc.
 *
 * Confidential computer software. Valid license from LG required for
 * possession, use or copying. Consistent with FAR 12.211 and 12.212,
 * Commercial Computer Software, Computer Software Documentation, and
 * Technical Data for Commercial Items are licensed to the U.S. Government
 * under vendor's standard commercial license.
 *
 * LICENSE@@@
 */

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
