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

#ifndef CORE_PARAMS_H_
#define CORE_PARAMS_H_

#include <iostream>

#include "interface/ISerializable.h"

using namespace std;

class Params : public ISerializable {
public:
    Params();
    virtual ~Params();

    virtual bool fromJson(JValue& json) override;
    virtual bool toJson(JValue& json) override;
    virtual void printDebug() override;

    const string& getRequster()
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

    const JValue& getExtra()
    {
        return m_extra;
    }

private:
    string m_requester;
    string m_action;
    string m_uri;
    JValue m_extra;
};

#endif /* CORE_PARAMS_H_ */
