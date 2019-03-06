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

#ifndef CORE_INTENTFILTER_H_
#define CORE_INTENTFILTER_H_

#include <iostream>

#include "interface/ISerializable.h"

using namespace std;

class IntentFilter : public ISerializable {
public:
    IntentFilter();
    virtual ~IntentFilter();

    virtual bool fromJson(JValue& json) override;
    virtual bool toJson(JValue& json) override;
    virtual void printDebug() override;

};

#endif /* CORE_INTENTFILTER_H_ */
