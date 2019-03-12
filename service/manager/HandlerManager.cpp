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

#include "util/Logger.h"

HandlerManager::HandlerManager()
{
    setName("HanderManager");

    // Test Data
    JValue json = pbnjson::Object();

    Handler handler;

    json.put("id", "test2");
    json.put("priority", 1);
    handler.fromJson(json);
    registerHandler(handler);

    json.put("id", "test3");
    json.put("priority", 2);
    handler.fromJson(json);
    registerHandler(handler);

    json.put("id", "test1");
    json.put("priority", 0);
    handler.fromJson(json);
    registerHandler(handler);
}

HandlerManager::~HandlerManager()
{
}

bool HandlerManager::onInitialization()
{
    return true;
}

bool HandlerManager::onFinalization()
{
    return true;
}

JValue HandlerManager::resolve(Intent intent)
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
    return true;
}

bool HandlerManager::registerHandler(Handler& handler)
{
    if (handler.getId().empty()) {
        Logger::warning("Id is null", m_name);
        return false;
    }
    if (hasHandler(handler.getId())) {
        Logger::warning("Same Id is already registered", m_name);
        return false;
    }

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if (it->getPriority() < handler.getPriority()) {
            m_handlers.insert(it, handler);
            return true;
        }
    }
    m_handlers.push_back(handler);
    return true;
}

bool HandlerManager::unregisterHandler(Handler& handler)
{
    if (handler.getId().empty()) {
        Logger::warning("Id is null", m_name);
        return false;
    }
    deque<Handler>::iterator it = findHandler(handler.getId());
    if (it == m_handlers.end()) {
        Logger::normal("The Id is not registered", m_name);
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
