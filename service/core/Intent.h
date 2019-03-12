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

#ifndef CORE_INTENT_H_
#define CORE_INTENT_H_

#include <iostream>

#include "interface/ISerializable.h"

using namespace std;

class Intent : public ISerializable {
friend class Handler;
public:
    Intent();
    virtual ~Intent();

    virtual bool fromJson(const JValue& json) override;
    virtual bool toJson(JValue& json) override;
    virtual void printDebug() override;

    const string& getRequester()
    {
        return m_requester;
    }

    const string& getAction()
    {
        return m_action;
    }

    const string& getUri()
    {
        return m_uri;
    }

    const string& getMimeType()
    {
        return m_mimeType;
    }

    const JValue& getExtra()
    {
        return m_extra;
    }

    const string& getResult()
    {
        return m_result;
    }

private:
    string m_requester;
    string m_action;
    string m_uri;
    string m_mimeType;
    JValue m_extra;
    string m_result;
};

#endif /* CORE_INTENT_H_ */
