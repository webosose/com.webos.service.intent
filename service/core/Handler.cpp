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

#include "Handler.h"

string Handler::toString(enum HandlerType type)
{
    switch(type) {
    case HandlerType_BuiltIn:
        return "builtIn";

    case HandlerType_Runtime:
        return "runtime";

    default:
        return "unknown";
    }
    return "unknown";
}

enum HandlerType Handler::toEnum(string type)
{
    if (type == "builtIn") {
        return HandlerType_BuiltIn;
    } else if (type == "runtime") {
        return HandlerType_Runtime;
    } else {
        return HandlerType_Unknown;
    }
}

Handler::Handler()
    : m_id("")
    , m_priority(0)
    , m_type(HandlerType_Unknown)
{
}

Handler::~Handler()
{
}

bool Handler::launch(Intent intent)
{
    return true;
}

bool Handler::isMatched(const Intent& intent)
{
    if (intent.m_action.empty()) {
        return true;
    }
    return true;
}

bool Handler::fromJson(const JValue& json)
{
    if (json.hasKey("id") && json["id"].isString()) {
        m_id = json["id"].asString();
    }
    if (json.hasKey("priority") && json["priority"].isNumber()) {
        m_priority = json["priority"].asNumber<int>();
    }
    if (json.hasKey("actions") && json["actions"].isArray()) {
        for (JValue action : json["actions"].items()) {
            m_actions.push_back(action.asString());
        }
    }
    if (json.hasKey("mimeTypes") && json["mimeTypes"].isArray()) {
        for (JValue mimeType : json["mimeTypes"].items()) {
            m_mimeTypes.push_back(mimeType.asString());
        }
    }
    if (json.hasKey("uris") && json["uris"].isArray()) {
        for (JValue uri : json["uris"].items()) {
            m_uris.push_back(uri.asString());
        }
    }
    if (json.hasKey("type") && json["type"].isString()) {
        m_type = toEnum(json["type"].asString());
    }
    return true;
}

bool Handler::toJson(JValue& json)
{
    json.put("id", m_id);
    json.put("priority", m_priority);

    JValue actions = pbnjson::Array();
    for (unsigned int i = 0; i < m_actions.size(); ++i) {
        actions.append(m_actions[i]);
    }
    json.put("actions", actions);

    JValue mimeTypes = pbnjson::Array();
    for (unsigned int i = 0; i < m_mimeTypes.size(); ++i) {
        actions.append(m_mimeTypes[i]);
    }
    json.put("mimeTypes", mimeTypes);

    JValue uris = pbnjson::Array();
    for (unsigned int i = 0; i < m_uris.size(); ++i) {
        actions.append(m_uris[i]);
    }
    json.put("uris", uris);
    json.put("type", toString(m_type));
    return true;
}

void Handler::printDebug()
{
    cout << "id : " << (m_id.empty() ? "NULL" : m_id) << endl;
    cout << "priority : " << m_priority << endl;

    cout << "actions :" << (m_actions.size() ? " " : "empty");
    for (unsigned int i = 0; i < m_actions.size(); ++i) {
        cout << "[" << m_actions[i] << "]";
    }
    cout << endl;

    cout << "mimeTypes :" << (m_mimeTypes.size() ? " " : "empty");
    for (unsigned int i = 0; i < m_mimeTypes.size(); ++i) {
        cout << "[" << m_mimeTypes[i] << "]";
    }
    cout << endl;

    cout << "uris :" << (m_uris.size() ? " " : "empty");
    for (unsigned int i = 0; i < m_uris.size(); ++i) {
        cout << "[" << m_uris[i] << "]";
    }
    cout << endl;
}
