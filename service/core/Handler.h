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

#ifndef CORE_HANDLER_H_
#define CORE_HANDLER_H_

#include <iostream>
#include <deque>
#include <map>
#include <network/uri.hpp>

#include "interface/ISerializable.h"
#include "core/Intent.h"

using namespace std;
using namespace network;

enum HandlerType {
    HandlerType_Unknown,
    HandlerType_AppInfo,
    HandlerType_Runtime,
};

class Handler : public ISerializable {
public:
    static string toString(enum HandlerType type);
    static enum HandlerType toEnum(string type);

    Handler();
    virtual ~Handler();

    virtual bool launch(Intent intent);
    virtual bool isMatched(const Intent& intent);

    // ISerializable
    virtual bool fromJson(const JValue& json) override;
    virtual bool toJson(JValue& json) override;

    void setId(string id)
    {
        m_id = id;
    }

    const string& getId() const
    {
        return m_id;
    }

    const int getPriority()
    {
        return m_priority;
    }

    const deque<string>& getActions()
    {
        return m_actions;
    }

    const deque<string>& getMimeTypes()
    {
        return m_mimeTypes;
    }

    const deque<uri>& getUris()
    {
        return m_uris;
    }

    void setType(HandlerType type)
    {
        m_type = type;
    }

    const HandlerType getType()
    {
        return m_type;
    }

private:
    static void getUniqueArray(const JValue& from, JValue& to);

    string m_id;
    int m_priority;

    deque<string> m_actions;
    deque<string> m_mimeTypes;
    deque<uri> m_uris;

    enum HandlerType m_type;
};

#endif /* CORE_HANDLER_H_ */
