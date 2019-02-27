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

#ifndef MANAGER_INTENTMANAGER_H_
#define MANAGER_INTENTMANAGER_H_

#include <iostream>

#include "interface/IManageable.h"

using namespace std;

class IntentManager : public IManageable<IntentManager> {
friend class IManageable<IntentManager>;
public:
    virtual ~IntentManager();

    virtual bool onInitialization() override;
    virtual bool onFInalization() override;

private:
    IntentManager();
};

#endif /* MANAGER_INTENTMANAGER_H_ */
