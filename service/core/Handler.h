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

#include "interface/ISerializable.h"

using namespace std;

enum HandlerType {
    HandlerType_Unknown,
    HandlerType_BuiltIn,
    HandlerType_Runtime,
};

class Handler : public ISerializable {
public:
    Handler();
    virtual ~Handler();

    virtual bool fromJson(JValue& json) override;
    virtual bool toJson(JValue& json) override;
    virtual void printDebug() override;

    const string& getId()
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

    const deque<string>& getUris()
    {
        return m_uris;
    }

private:
    string m_id;
    int m_priority;

    deque<string> m_actions;
    deque<string> m_mimeTypes;
    deque<string> m_uris;

    enum HandlerType m_type;
};

#endif /* CORE_HANDLER_H_ */
