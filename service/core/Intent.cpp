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

#include "util/Logger.h"

Intent::Intent()
    : m_requester("")
    , m_action("")
    , m_mimeType("")
    , m_result("")
    , m_uri("")
    , m_chooser(false)
{
    setClassName("Intent");
    m_extra = pbnjson::Object();
}

Intent::~Intent()
{

}

bool Intent::fromJson(const JValue& json)
{
    if (json.hasKey("id") && json["id"].isString()) {
        m_id = json["id"].asString();
    }
    if (json.hasKey("requester") && json["requester"].isString()) {
        m_requester = json["requester"].asString();
    }
    if (json.hasKey("action") && json["action"].isString()) {
        m_action = json["action"].asString();
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
    if (json.hasKey("chooser") && json["chooser"].isBoolean()) {
        m_chooser = json["chooser"].asBool();
    }

    try {
        if (json.hasKey("uri") && json["uri"].isString()) {
            m_uri = uri(json["uri"].asString());
        }
    } catch (network::uri_syntax_error& ex) {
        Logger::warning(getClassName(), ex.what());
        return false;
    }
    return true;
}

bool Intent::toJson(JValue& json)
{
    json.put("id", m_id);
    json.put("requester", m_requester);
    json.put("action", m_action);
    json.put("uri", m_uri.string());
    json.put("mimeType", m_mimeType);
    json.put("result", m_result);
    json.put("extra", m_extra.duplicate());
    return true;
}
