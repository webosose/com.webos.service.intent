// Copyright (c) 2020 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#ifndef UTIL_NATIVEPROCESS_H_
#define UTIL_NATIVEPROCESS_H_

#include <iostream>
#include <vector>
#include <map>
#include <glib.h>

using namespace std;

class NativeProcess {
public:
    NativeProcess();
    virtual ~NativeProcess();

    void setWorkingDirectory(const string& directory)
    {
        m_workingDirectory = directory;
    }

    void setCommand(const string& command)
    {
        m_command = command;
    }
    const string& getCommand()
    {
        return m_command;
    }

    void addArgument(const string& argument);
    void addArgument(const string& option, const string& value);
    void addEnv(map<string, string>& environments);
    void addEnv(const string& variable, const string& value);

    pid_t getPid() const
    {
        return m_pid;
    }
    void setPid(pid_t pid)
    {
        m_pid = pid;
    }

    void openLogfile(const string& logfile);
    void closeLogfile();

    bool run();
    bool term();
    bool kill();

    void track()
    {
        m_isTracked = true;
    }

    bool isTracked()
    {
        return m_isTracked;
    }

private:
    static const string CLASS_NAME;
    static const int MAX_ARGS = 16;
    static const int MAX_ENVP = 64;

    static void convertEnvToStr(map<string, string>& src, vector<string>& dest);
    static void prepareSpawn(gpointer user_data);

    string m_workingDirectory;
    string m_command;

    vector<string> m_arguments;
    map<string, string> m_environments;

    pid_t m_pid;
    gint m_logfile;

    bool m_isTracked;

};

#endif /* UTIL_NATIVEPROCESS_H_ */
