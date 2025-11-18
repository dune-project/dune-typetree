# SPDX-FileCopyrightInfo: Copyright © DUNE Project contributors, see file LICENSE.md in module root
# SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-GPL-2.0-only-with-PDELab-exception

cmake_minimum_required(VERSION 3.16)
project("dune-typetree" C CXX)

if(NOT (dune-common_DIR OR dune-common_ROOT OR
        "${CMAKE_PREFIX_PATH}" MATCHES ".*dune-common.*"))
    string(REPLACE  ${CMAKE_PROJECT_NAME} dune-common dune-common_DIR
      ${PROJECT_BINARY_DIR})
endif()

#find dune-common and set the module path
find_package(dune-common REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${dune-common_MODULE_PATH}
  "${PROJECT_SOURCE_DIR}/cmake/modules")

#include the dune macros
include(DuneMacros)

# deactivate global include-directories
dune_policy(SET DP_DEFAULT_INCLUDE_DIRS NEW)

# deactivate global calls to add_dune_all_flags in tests
dune_policy(SET DP_TEST_ADD_ALL_FLAGS NEW)

# deactivate suggested module dependencies being required
dune_policy(SET DP_SUGGESTED_MODULE_DEPENDENCIES_REQUIRED_DOWNSTREAM NEW)

# start a dune project with information from dune.module
dune_project()
dune_add_library(dunetypetree INTERFACE
  EXPORT_NAME TypeTree
  LINK_LIBRARIES Dune::Common)

# set include directories for dunetypetree library
dune_default_include_directories(dunetypetree INTERFACE)

add_subdirectory(doc)
add_subdirectory(dune)
add_subdirectory(test)

# finalize the dune project, e.g. generating config.h etc.
finalize_dune_project()
