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

#include "../util/Proc.h"

bool Proc::getMemoryInfo(long& total, long& available)
{
    string type;
    long value;
    string kb;

    ifstream meminfo("/proc/meminfo");
    while (true) {
        meminfo >> type >> value >> kb;

        if (type == "MemTotal:") {
            total = value / 1024;
        }
        if (type == "MemAvailable:") {
            available = value / 1024;
            break;
        }
    }
    meminfo.close();
    return true;
}
