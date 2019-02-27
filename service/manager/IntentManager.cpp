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

#include "IntentManager.h"
#include "util/Logger.h"

IntentManager::IntentManager()
{
    setName("IntentManager");
}

IntentManager::~IntentManager()
{
}

bool IntentManager::onInitialization()
{
    return true;
}

bool IntentManager::onFInalization()
{
    return true;
}
