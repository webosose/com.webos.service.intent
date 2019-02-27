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

#include "manager/HandlerManager.h"
#include "manager/IntentManager.h"
#include "manager/StorageManager.h"

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

    HandlerManager::getInstance().initialize(s_mainloop);
    IntentManager::getInstance().initialize(s_mainloop);
    StorageManager::getInstance().initialize(s_mainloop);

    g_main_loop_run(s_mainloop);

    HandlerManager::getInstance().initialize(s_mainloop);
    IntentManager::getInstance().initialize(s_mainloop);
    StorageManager::getInstance().initialize(s_mainloop);

    g_main_loop_unref(s_mainloop);

    return 0;
}