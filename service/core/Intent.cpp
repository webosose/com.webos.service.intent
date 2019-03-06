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

#include "Intent.h"

Intent::Intent()
    : m_requester("")
    , m_action("")
    , m_uri("")
    , m_mimeType("")
    , m_result("")
{
    m_extra = pbnjson::Object();
}

Intent::~Intent()
{

}

bool Intent::fromJson(JValue& json)
{
    if (json.hasKey("requester") && json["requester"].isString()) {
        m_requester = json["requester"].asString();
    }
    if (json.hasKey("action") && json["action"].isString()) {
        m_action = json["action"].asString();
    }
    if (json.hasKey("uri") && json["uri"].isString()) {
        m_uri = json["uri"].asString();
    }
    if (json.hasKey("mimeType") && json["mimeType"].isString()) {
        m_mimeType = json["mimeType"].asString();
    }
    if (json.hasKey("result") && json["result"].isString()) {
        m_result = json["result"].asString();
    }
    if (json.hasKey("extra") && json["extra"].isObject()) {
        m_extra = json["extra"].duplicate();
    }
    return true;
}

bool Intent::toJson(JValue& json)
{
    json.put("requester", m_requester);
    json.put("action", m_action);
    json.put("uri", m_uri);
    json.put("mimeType", m_mimeType);
    json.put("result", m_result);
    json.put("extra", m_extra.duplicate());
    return true;
}

void Intent::printDebug()
{
    cout << "requester : " << (m_requester.empty() ? "NULL" : m_requester) << endl;
    cout << "action : " << (m_action.empty() ? "NULL" : m_action) << endl;
    cout << "uri : " << (m_uri.empty() ? "NULL" : m_uri) << endl;
    cout << "mimeType : " << (m_mimeType.empty() ? "NULL" : m_mimeType) << endl;
    cout << "result : " << (m_result.empty() ? "NULL" : m_result) << endl;
    cout << "extra : " << (m_extra.objectSize() == 0 ? "NULL" : m_extra.stringify("    ")) << endl;
}
