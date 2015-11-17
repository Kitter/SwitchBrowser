cmake_minimum_required(VERSION 3.2)
project(SwitchBrowser)

# set version
set(COLLECTOR_VERSION "0.1.2")
#
# Debugging Options
#
SET(CMAKE_VERBOSE_MAKEFILE 0) # Use 1 for debugging, 0 for release

#SET(CMAKE_INSTALL_PREFIX /usr/local)
SET(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/target)

#local cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules" ${CMAKE_MODULE_PATH})

# Initialize CXXFLAGS.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "$ENV{CXXFLAGS} -Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "$ENV{CXXFLAGS} -O2 -g")

#
# Project Output Paths
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/plugins)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/archive)

# for multi-config builds (e.g. msvc xcode)
foreach (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/plugins)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_BINARY_DIR}/archive)
endforeach (OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

message("Current CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")


#
# Locate Project Prerequisites
#
SET(Boost_ADDITIONAL_VERSIONS "1.53" "1.54" "1.55")
FIND_PACKAGE(Boost COMPONENTS "iostreams" "thread" "system" "regex" REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

FIND_PACKAGE(NETSNMP)
INCLUDE_DIRECTORIES(${NETSNMP_INCLUDE_DIRS})
LINK_DIRECTORIES(${NETSNMP_LIBRARIES})


# Copy all config files
FILE(GLOB MIBS_FILES mibs/cisco-mibs/*.*)
FOREACH(_filepath ${MIBS_FILES})
    get_filename_component(var ${_filepath} NAME)
    configure_file(${_filepath}  mibs/cisco-mibs/${var} COPYONLY)
ENDFOREACH()
FILE(GLOB MIBS_FILES mibs/ruijie-mibs/*.*)
FOREACH(_filepath ${MIBS_FILES})
    get_filename_component(var ${_filepath} NAME)
    configure_file(${_filepath}  mibs/ruijie-mibs/${var} COPYONLY)
ENDFOREACH()


set(TEST_SRCS
    snmp_utils.cpp
    switch_fetch.cpp
    TEST.cpp
        )

add_executable(TEST ${TEST_SRCS})
target_link_libraries(TEST netsnmp)
