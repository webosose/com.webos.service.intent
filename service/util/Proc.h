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

#ifndef UTIL_PROC_H_
#define UTIL_PROC_H_

#include <iostream>
#include <fstream>

using namespace std;

enum OverCommitPolicy {
    OverCommitPolicy_Default

};

class Proc {
public:
    Proc() {}
    virtual ~Proc() {}

    static bool getMemoryInfo(long& total, long& available);
};

#endif /* UTIL_PROC_H_ */
