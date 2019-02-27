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
#include <queue>

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

private:
    string m_id;
    enum HandlerType m_type;

    queue<string> m_actions;
    queue<string> m_mimeTypes;
    queue<string> m_uris;
};

#endif /* CORE_HANDLER_H_ */
