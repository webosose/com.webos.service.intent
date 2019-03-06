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

Handler::Handler()
    : m_id("")
    , m_priority(0)
    , m_type(HandlerType_Unknown)
{
}

Handler::~Handler()
{
}

bool Handler::fromJson(JValue& json)
{
    if (json.hasKey("id") && json["id"].isString()) {
        m_id = json["id"].asString();
    }
    if (json.hasKey("priority") && json["priority"].isNumber()) {
        m_priority = json["action"].asNumber<int>();
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
    return true;
}

bool Handler::toJson(JValue& json)
{
    return true;
}

void Handler::printDebug()
{
    cout << "id : " << (m_id.empty() ? "NULL" : m_id) << endl;
    cout << "priority : " << m_priority << endl;

    cout << "actions :" << (m_actions.size() ? " " : "empty");
    for (unsigned int i = 0; i < m_actions.size(); i++) {
        cout << "[" << m_actions[i] << "]";
    }
    cout << endl;

    cout << "mimeTypes :" << (m_mimeTypes.size() ? " " : "empty");
    for (unsigned int i = 0; i < m_mimeTypes.size(); i++) {
        cout << "[" << m_mimeTypes[i] << "]";
    }
    cout << endl;

    cout << "uris :" << (m_uris.size() ? " " : "empty");
    for (unsigned int i = 0; i < m_uris.size(); i++) {
        cout << "[" << m_uris[i] << "]";
    }
    cout << endl;
}
