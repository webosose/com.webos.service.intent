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

#include "ConfigManager.h"
#include "HandlerManager.h"

#include <algorithm>

#include "util/Logger.h"

HandlerManager::HandlerManager()
{
    setName("HanderManager");
}

HandlerManager::~HandlerManager()
{
}

void HandlerManager::loadConfig(const JValue& json)
{
    for (JValue item : json["handlers"].items()) {
        Handler handler;
        handler.fromJson(item);

        if (handler.getType() != HandlerType_AppInfo) {
            registerHandler(handler);
        }
    }
}

bool HandlerManager::onInitialization()
{
    if (ConfigManager::getInstance().isTest()) {
        loadConfig(ConfigManager::getInstance().getDebugDB());
        Logger::info(m_name, "Load buildtime configurations");
    }
    loadConfig(ConfigManager::getInstance().getRuntimeDB());
    Logger::info(m_name, "Load runtime configurations");
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
        Logger::info(m_name, "Save changed runtime handlers");
    }
    return true;
}

bool HandlerManager::launch(Intent& intent)
{
    deque<Handler>::iterator it;
    it = find_if(m_handlers.begin(), m_handlers.end(),
                 [&] (Handler& handler)
                 {
                    return handler.isMatched(intent);
                 });
    if (it == m_handlers.end()) {
        return false;
    }
    Logger::info(m_name, it->getId(), "Launch target handler");
    // TODO : Need to call SAM 'launch' API
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
        Logger::warning(m_name, handler.getId(), "Cannot find handler");
        return false;
    }

    // Currently, setHandler API overwrites all internal information
    JValue json = pbnjson::Object();
    handler.toJson(json);
    handler.setType(HandlerType_Runtime);
    it->fromJson(json);
    return true;
}

bool HandlerManager::registerHandler(Handler& handler)
{
    if (handler.getId().empty()) {
        Logger::warning(m_name, "Id is null");
        return false;
    }
    if (hasHandler(handler.getId())) {
        if (handler.getType() == HandlerType_AppInfo) {
            Logger::info(m_name, handler.getId(), "'runtime' handler is already registered");
            return true;
        }
        Logger::warning(m_name, handler.getId(), "Same Id is already registered.");
        return false;
    }
    if (handler.getActions().size() == 0) {
        Logger::warning(m_name, handler.getId(), "'Actions' is required parameter for registration");
        return false;
    }

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if (it->getPriority() < handler.getPriority()) {
            m_handlers.insert(it, handler);
            return true;
        }
    }
    m_handlers.push_back(handler);
    Logger::info(m_name, handler.getId(), "Handler is registered");
    return true;
}

bool HandlerManager::unregisterHandler(Handler& handler)
{
    if (handler.getId().empty()) {
        Logger::warning(m_name, "Id is null");
        return false;
    }
    deque<Handler>::iterator it = findHandler(handler.getId());
    if (it == m_handlers.end()) {
        Logger::info(m_name, handler.getId(), "The Id is not registered");
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
