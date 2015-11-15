#.rst:
# FindNETSNMP
# --------
#
# Find NETSNMP
#
# Find the native CURL headers and libraries.
#
# ::
#
#   NETSNMP_INCLUDE_DIRS   - where to find json/josn.h, etc.
#   NETSNMP_LIBRARIES      - List of libraries when using json.
#   NETSNMP_FOUND          - True if json found.
#   NETSNMP_VERSION_STRING - the version of json found (since CMake 2.8.8)

#=============================================================================
# Copyright 2006-2009 Kitware, Inc.
# Copyright 2012 Rolf Eike Beer <eike@sf-mail.de>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# Look for the header file.
set(_NETSNMP_SEARCHES)

# Search ZLIB_ROOT first if it is set.
if(NETSNMP_ROOT)
  set(_NETSNMP_SEARCH_ROOT PATHS ${NETSNMP_ROOT} NO_DEFAULT_PATH)
  list(APPEND _NETSNMP_SEARCHES _NETSNMP_SEARCH_ROOT)
endif()

# Normal search.
set(_NETSNMP_SEARCH_NORMAL
  PATHS "[HKEY_LOCAL_MACHINE\\SOFTWARE\\GnuWin32\\;InstallPath]"
        "$ENV{PROGRAMFILES}"
        /usr/local
  )
list(APPEND _NETSNMP_SEARCHES _NETSNMP_SEARCH_NORMAL)

#set(NETSNMP_NAMES NETSNMP)

# Try each search configuration.
foreach(search ${_NETSNMP_SEARCHES})
  find_path(NETSNMP_INCLUDE_DIR NAMES net-snmp/net-snmp-includes.h        ${${search}} PATH_SUFFIXES include)
  find_library(NETSNMP_LIBRARY  NAMES netsnmp           ${${search}} PATH_SUFFIXES lib)
endforeach()

mark_as_advanced(NETSNMP_LIBRARY NETSNMP_INCLUDE_DIR)

if(NETSNMP_INCLUDE_DIR AND EXISTS "${NETSNMP_INCLUDE_DIR}/NETSNMP/version.h")
    file(STRINGS "${NETSNMP_INCLUDE_DIR}/NETSNMP/version.h" NETSNMP_H REGEX "^.*NETSNMP_VERSION_STRING.*\"")

    string(REGEX REPLACE "^.*NETSNMP_VERSION_STRING \"([0-9]+).*$" "\\1" NETSNMP_VERSION_MAJOR "${NETSNMP_H}")
    string(REGEX REPLACE "^.*NETSNMP_VERSION_STRING \"[0-9]+\\.([0-9]+).*$" "\\1" NETSNMP_VERSION_MINOR  "${NETSNMP_H}")
    string(REGEX REPLACE "^.*NETSNMP_VERSION_STRING \"[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" NETSNMP_VERSION_PATCH "${NETSNMP_H}")
    set(NETSNMP_VERSION_STRING "${NETSNMP_VERSION_MAJOR}.${NETSNMP_VERSION_MINOR}.${NETSNMP_VERSION_PATCH}")

    # only append a TWEAK version if it exists:
    set(NETSNMP_VERSION_TWEAK "")
    if( "${NETSNMP_H}" MATCHES "^.*NETSNMP_VERSION_STRING \"[0-9]+\\.[0-9]+\\.[0-9]+\\.([0-9]+).*$")
        set(NETSNMP_VERSION_TWEAK "${CMAKE_MATCH_1}")
        set(NETSNMP_VERSION_STRING "${NETSNMP_VERSION_STRING}.${NETSNMP_VERSION_TWEAK}")
    endif()

    set(NETSNMP_MAJOR_VERSION "${NETSNMP_VERSION_MAJOR}")
    set(NETSNMP_MINOR_VERSION "${NETSNMP_VERSION_MINOR}")
    set(NETSNMP_PATCH_VERSION "${NETSNMP_VERSION_PATCH}")
endif()

# handle the QUIETLY and REQUIRED arguments and set json_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NETSNMP
                                  REQUIRED_VARS NETSNMP_LIBRARY NETSNMP_INCLUDE_DIR
                                  VERSION_VAR NETSNMP_VERSION_STRING)

if(NETSNMP_FOUND)
  set(NETSNMP_LIBRARIES ${NETSNMP_LIBRARY})
  set(NETSNMP_INCLUDE_DIRS ${NETSNMP_INCLUDE_DIR})
endif()
