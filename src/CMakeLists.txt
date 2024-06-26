# Copyright (c) 2020-2024 LG Electronics, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

include(FindPkgConfig)

project(com.webos.service.intent C CXX)

pkg_check_modules(GLIB2 REQUIRED glib-2.0)
include_directories(${GLIB2_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${GLIB2_CFLAGS_OTHER})

pkg_check_modules(LUNASERVICE2 REQUIRED luna-service2)
include_directories(${LUNASERVICE2_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${LUNASERVICE2_CFLAGS_OTHER})

pkg_check_modules(LUNASERVICE2CPP REQUIRED luna-service2++)
include_directories(${LUNASERVICE2CPP_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${LUNASERVICE2CPP_CFLAGS_OTHER})

pkg_check_modules(PBNJSON_C REQUIRED pbnjson_c)
include_directories(${PBNJSON_C_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${PBNJSON_C_CFLAGS_OTHER})

pkg_check_modules(PBNJSON_CPP REQUIRED pbnjson_cpp)
include_directories(${PBNJSON_CPP_INCLUDE_DIRS})
webos_add_compiler_flags(ALL ${PBNJSON_CPP_CFLAGS_OTHER})

# Environment
file(GLOB_RECURSE SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Compile
webos_add_compiler_flags(ALL CXX -std=c++0x)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${CMAKE_PROJECT_NAME} ${SRC})

# Link
set(LIBS
    ${GLIB2_LDFLAGS}
    ${LUNASERVICE2_LDFLAGS}
    ${LUNASERVICE2CPP_LDFLAGS}
    ${PBNJSON_C_LDFLAGS}
    ${PBNJSON_CPP_LDFLAGS}
)
target_link_libraries(${CMAKE_PROJECT_NAME} ${LIBS})

# Install
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION ${WEBOS_INSTALL_SBINDIR})
