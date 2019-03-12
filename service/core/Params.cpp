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

#include "Params.h"

Params::Params()
{
}

Params::~Params()
{
}

bool Params::fromJson(const JValue& json)
{
    if (json.hasKey("requester") && json["requester"].isString()) {
        m_requester = json["requester"].asString();
    }
    if (json.hasKey("action") && json["action"].isString()) {
        m_action = json["action"].asString();
    }
    if (json.hasKey("uri") && json["m_uri"].isString()) {
        m_uri = json["m_uri"].asString();
    }
    if (json.hasKey("extra") && json["extra"].isObject()) {
        m_extra = json["extra"].duplicate();
    }
    return true;
}

bool Params::toJson(JValue& json)
{
    json.put("requester", m_requester);
    json.put("action", m_action);
    json.put("uri", m_uri);
    json.put("extra", m_extra.duplicate());
    return true;
}

void Params::printDebug()
{
    cout << "requester : " << (m_requester.empty() ? "NULL" : m_requester) << endl;
    cout << "action : " << (m_action.empty() ? "NULL" : m_action) << endl;
    cout << "uri : " << (m_uri.empty() ? "NULL" : m_uri) << endl;
    cout << "extra : " << (m_extra.objectSize() == 0 ? "NULL" : m_extra.stringify("    ")) << endl;
}
