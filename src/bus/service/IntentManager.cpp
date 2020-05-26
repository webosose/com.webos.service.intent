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

#include "IntentManager.h"

#include <algorithm>

Intent IntentManager::CHOOSER;

IntentManager::IntentManager()
{
    setClassName("HanderManager");
    CHOOSER.setAction("action_chooser");
}

IntentManager::~IntentManager()
{
}

void IntentManager::loadConfig(const JValue& json)
{
    for (JValue item : json["handlers"].items()) {
        HandlerPtr handler = make_shared<Handler>();
        handler->fromJson(item);
        Handlers::getInstance().add(handler, HandlerType_AppInfo);
    }
}

bool IntentManager::onInitialization()
{
    if (ConfigFile::getInstance().isTest()) {
        loadConfig(ConfigFile::getInstance().getDebugDB());
        Logger::info(getClassName(), __FUNCTION__, "Load debug configurations");
    }
    loadConfig(ConfigFile::getInstance().getRuntimeDB());
    Logger::info(getClassName(), __FUNCTION__, "Load runtime configurations");
    ready();
    return true;
}

bool IntentManager::onFinalization()
{
    return true;
}

bool IntentManager::launch(Intent& intent)
{
    Intent *targetIntent = nullptr;
    if (intent.chooser()) {
        targetIntent = &CHOOSER;
    } else {
        targetIntent = &intent;
    }
    HandlerPtr handler = Handlers::getInstance().findHandler(*targetIntent);
    if (handler == nullptr) {
        return false;
    }
    Logger::info(getClassName(), handler->getId(), "Launch target handler");
    SAM::getInstance().launch(intent, handler);
    return true;
}

JValue IntentManager::resolve(Intent& intent)
{
    JValue json = pbnjson::Array();
    Handlers::getInstance().toJson(json, intent);
    return json;
}

JValue IntentManager::getAllHandlers()
{
    JValue json = pbnjson::Array();
    Handlers::getInstance().toJson(json);
    return json;
}

JValue IntentManager::getHandler(const string& id)
{
    JValue json = pbnjson::Object();
    Handlers::getInstance().toJson(json, id);
    return json;
}
