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

#include "HandlerManager.h"

#include <algorithm>

#include "ApplicationManager.h"
#include "IntentManager.h"
#include "ConfigManager.h"
#include "util/Logger.h"

Intent HandlerManager::CHOOSER;

HandlerManager::HandlerManager()
{
    setClassName("HanderManager");
    CHOOSER.setAction("action_chooser");
}

HandlerManager::~HandlerManager()
{
}

void HandlerManager::loadConfig(const JValue& json)
{
    for (JValue item : json["handlers"].items()) {
        Handler handler;
        handler.fromJson(item);

        registerHandler(handler);
    }
}

bool HandlerManager::onInitialization()
{
    if (ConfigManager::getInstance().isTest()) {
        loadConfig(ConfigManager::getInstance().getDebugDB());
        Logger::info(getClassName(), "Load debug configurations");
    }
    loadConfig(ConfigManager::getInstance().getRuntimeDB());
    Logger::info(getClassName(), "Load runtime configurations");
    ready();
    return true;
}

bool HandlerManager::onFinalization()
{
    JValue handlers = pbnjson::Array();
    for (unsigned int i = 0; i < m_handlers.size(); ++i) {
        if (m_handlers[i].getType() == HandlerType_Runtime) {
            JValue handler = pbnjson::Object();
            m_handlers[i].toJson(handler);
            handlers.append(handler);
        }
    }

    if (ConfigManager::getInstance().getRuntimeDB()["handlers"] != handlers) {
        ConfigManager::getInstance().getRuntimeDB().put("handlers", handlers);
        Logger::info(getClassName(), "Save changed runtime handlers");
    }
    return true;
}

bool HandlerManager::launch(Intent& intent)
{
    Intent *targetIntent = nullptr;
    if (intent.chooser()) {
        targetIntent = &CHOOSER;
    } else {
        targetIntent = &intent;
    }
    deque<Handler>::iterator it;
    it = find_if(m_handlers.begin(), m_handlers.end(),
                 [&] (Handler& handler)
                 {
                    return handler.isMatched(*targetIntent);
                 });
    if (it == m_handlers.end()) {
        return false;
    }
    Logger::info(getClassName(), it->getId(), "Launch target handler");
    ApplicationManager::getInstance().launch(intent, *it);
    return true;
}

JValue HandlerManager::resolve(Intent& intent)
{
    deque<Handler> handlers;
    std::copy_if(m_handlers.begin(), m_handlers.end(), back_inserter(handlers),
        [&](Handler& handler)
        {
            return handler.isMatched(intent);
        }
    );

    JValue json = pbnjson::Array();
    for(unsigned i = 0; i < handlers.size(); i++) {
        JValue item = pbnjson::Object();
        handlers[i].toJson(item);
        json.append(item);
    }
    return json;
}

JValue HandlerManager::getAllHandlers()
{
    JValue json = pbnjson::Array();
    for(unsigned i = 0; i < m_handlers.size(); i++) {
        JValue item = pbnjson::Object();
        m_handlers[i].toJson(item);
        json.append(item);
    }
    return json;
}

JValue HandlerManager::getHandler(const string& id)
{
    JValue json = pbnjson::Object();

    deque<Handler>::iterator it = findHandler(id);
    if (it != m_handlers.end()) {
        it->toJson(json);
    }
    return json;
}

bool HandlerManager::setHandler(Handler& handler)
{
    deque<Handler>::iterator it = findHandler(handler.getId());
    if (it == m_handlers.end()) {
        Logger::warning(getClassName(), handler.getId(), "Cannot find handler");
        return false;
    }

    if (it->getType() != handler.getType()) {
        Logger::warning(getClassName(), handler.getId(), "Cannot overwrite existing handler");
        return false;
    }

    *it = handler;
    return true;
}

bool HandlerManager::registerHandler(Handler& handler)
{
    if (handler.getId().empty()) {
        Logger::warning(getClassName(), "Id is null");
        return false;
    }
    if (handler.getType() == HandlerType_Unknown) {
        Logger::warning(getClassName(), "Type should not be unknown");
        return false;
    }
    if (hasHandler(handler.getId())) {
        if (handler.getType() == HandlerType_AppInfo) {
            Logger::info(getClassName(), handler.getId(), "'runtime' handler is already registered");
            return true;
        }
        Logger::warning(getClassName(), handler.getId(), "Same Id is already registered.");
        return false;
    }
    if (handler.getActions().size() == 0) {
        Logger::warning(getClassName(), handler.getId(), "'Actions' is required parameter for registration");
        return false;
    }

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if (it->getPriority() < handler.getPriority()) {
            m_handlers.insert(it, handler);
            return true;
        }
    }
    m_handlers.push_back(handler);
    Logger::info(getClassName(), handler.getId(), "Handler is registered");
    return true;
}

bool HandlerManager::unregisterHandler(Handler& handler)
{
    if (handler.getId().empty()) {
        Logger::warning(getClassName(), "Id is null");
        return false;
    }
    deque<Handler>::iterator it = findHandler(handler.getId());
    if (it == m_handlers.end()) {
        Logger::info(getClassName(), handler.getId(), "The Id is not registered");
        return false;
    }
    if (it->getType() == HandlerType_AppInfo) {
        Logger::warning(getClassName(), handler.getId(), "Cannot unregister appinfo handler");
        return false;
    }
    m_handlers.erase(it);
    return true;
}

bool HandlerManager::hasHandler(const string& id)
{
    deque<Handler>::iterator it;
    it = find_if(m_handlers.begin(), m_handlers.end(),
                 [&] (const Handler& handler) { return handler.getId() == id; } );
    if (it == m_handlers.end()) {
        return false;
    }
    return true;
}

deque<Handler>::iterator HandlerManager::findHandler(const string& id)
{
    deque<Handler>::iterator it;
    it = find_if(m_handlers.begin(), m_handlers.end(),
                 [&] (const Handler& handler) { return handler.getId() == id; } );
    return it;
}
