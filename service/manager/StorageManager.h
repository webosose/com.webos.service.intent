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

#ifndef MANAGER_STORAGEMANAGER_H_
#define MANAGER_STORAGEMANAGER_H_

#include <iostream>

#include <pbnjson.hpp>

#include "interface/IManageable.h"

using namespace std;
using namespace pbnjson;

class StorageManager : public IManageable<StorageManager> {
friend class IManageable<StorageManager>;
public:
    virtual ~StorageManager();

    virtual bool onInitialization() override;
    virtual bool onFinalization() override;

    virtual JValue& get();
    virtual void sync();

private:
    static const string PATH;

    StorageManager();

    JValue m_database;
};

#endif /* MANAGER_STORAGEMANAGER_H_ */
