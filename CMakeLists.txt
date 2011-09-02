cmake_minimum_required(VERSION 2.6)

##################################################
# Find extra cmake modules dirs
#
# You can clone all modules from https://ssl.izenesoft.cn/projects/cmake
#
# Clone it and set environment variable EXTRA_CMAKE_MODULES_DIRS point to it.
#
SET(ENV_EXTRA_CMAKE_MODULES_DIRS $ENV{EXTRA_CMAKE_MODULES_DIRS})
IF(ENV_EXTRA_CMAKE_MODULES_DIRS)
  SET(EXTRA_CMAKE_MODULES_DIRS ${EXTRA_CMAKE_MODULES_DIRS} ${ENV_EXTRA_CMAKE_MODULES_DIRS})
ENDIF(ENV_EXTRA_CMAKE_MODULES_DIRS)
IF(NOT EXTRA_CMAKE_MODULES_DIRS)
  MESSAGE(FATAL_ERROR
    "Cannot find EXTRA_CMAKE_MODULES_DIRS.
1. Please clone it from https://ssl.izenesoft.cn/cmake.
2. Then set EXTRA_CMAKE_MODULES_DIRS through environment variable

       export EXTRA_CMAKE_MODULES_DIRS=/path/to/cmake

   You can put it in your .bashrc so it is loaded automatically.

   or cmake options

       cmake  -DEXTRA_CMAKE_MODULES_DIRS=/path/to/cmake (other options)
")
ENDIF(NOT EXTRA_CMAKE_MODULES_DIRS)
SET(CMAKE_MODULE_PATH ${EXTRA_CMAKE_MODULES_DIRS})

project(ILPLIB)

#set(CMAKE_VERBOSE_MAKEFILE on)

# In CMake, compiler flags can be grouped in Configurations
#

# - Set default configurations definition, also add configuration Debugfull and Profile.
#
# Debugfull adds more debug information, and Profile adds profiling information as well.
#

IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo)
ENDIF (NOT CMAKE_BUILD_TYPE)

MESSAGE(STATUS "CMAKE_COMPILER_IS_GNUCXX: " ${CMAKE_COMPILER_IS_GNUCXX})
IF (CMAKE_COMPILER_IS_GNUCXX)
  INCLUDE(MacroSetBuildTypes)

  MACRO_SET_BUILD_TYPES(
    RelWithDebInfo
    "-O2 -g -Wno-deprecated -DNDEBUG -fPIC"
    "Flags used by the compiler during Release with Debug Info builds."

    Release
    "-O2 -Wno-deprecated -DNDEBUG -fPIC"
    "Flags used by the compiler during release builds"

    Debug
    "-g -O0 -fno-reorder-blocks -fno-schedule-insns -fno-inline -Wno-deprecated -fPIC"
    "Flags used by the compiler during debug builds."

    Profile
    "-g3 -fno-inline -ftest-coverage -fprofile-arcs -Wno-deprecated -fPIC"
    "Profile build type"
  )
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

INCLUDE(MacroSetDefaultBuildType)
MACRO_SET_DEFAULT_BUILD_TYPE(RelWithDebInfo)

INCLUDE(SetCompilerWarningAll)

# Write to cache
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING
  "Flags used by the compiler during all build types." FORCE)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING
  "Flags used by the compiler during all build types." FORCE)

##################################################
# Find extra cmake modules dirs
#
# You can clone all modules from izenesoft.com:/git/cmake.git.
# 
# Copy it to your install dir or change the variable
# EXTRA_CMAKE_MODULES_DIRS.
#
IF(IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/cmake")
  SET(EXTRA_CMAKE_MODULES_DIRS ${EXTRA_CMAKE_MODULES_DIRS} "${CMAKE_INSTALL_PREFIX}/cmake")
ENDIF(IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/cmake")
SET(ENV_EXTRA_CMAKE_MODULES_DIRS $ENV{EXTRA_CMAKE_MODULES_DIRS})
IF(ENV_EXTRA_CMAKE_MODULES_DIRS)
  SET(EXTRA_CMAKE_MODULES_DIRS ${EXTRA_CMAKE_MODULES_DIRS} ${ENV_EXTRA_CMAKE_MODULES_DIRS})
ENDIF(ENV_EXTRA_CMAKE_MODULES_DIRS)
IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake")
  SET(EXTRA_CMAKE_MODULES_DIRS ${EXTRA_CMAKE_MODULES_DIRS} "${CMAKE_SOURCE_DIR}/cmake")
ENDIF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/cmake")
IF(NOT EXTRA_CMAKE_MODULES_DIRS)
  MESSAGE(FATAL_ERROR
    "Cannot find EXTRA_CMAKE_MODULES_DIRS.
Please clone it from izenesoft.com:/git/cmake.git. Then set EXTRA_CMAKE_MODULES_DIRS (-DEXTRA_CMAKE_MODULES_DIRS=/path/to/cmake)to path of the cloned repository. You also can copy the cloned cmakedirectory to ${CMAKE_INSTALL_PREFIX} or ${CMAKE_SOURCE_DIR}")
ENDIF(NOT EXTRA_CMAKE_MODULES_DIRS)
SET(CMAKE_MODULE_PATH ${EXTRA_CMAKE_MODULES_DIRS})

IF (CMAKE_COMPILER_IS_GNUCXX)
add_definitions("-Wall -Wno-deprecated")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)


# Boost package
SET(Boost_ADDITIONAL_VERSIONS 1.39 1.39.0 1.38 1.38.0 1.37 1.37.0)
FIND_PACKAGE(Boost 1.36 COMPONENTS
    program_options thread serialization filesystem system)

IF(Boost_FOUND)
    MESSAGE(STATUS "BOOST:")
    MESSAGE(STATUS "  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    MESSAGE(STATUS "  Boost_LIBRARIES: ${Boost_LIBRARIES}")
    MESSAGE(STATUS "  version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
    include_directories(${Boost_INCLUDE_DIR})
ELSE(Boost_FOUND)
    MESSAGE(STATUS "BOOST: not found")
ENDIF(Boost_FOUND)

FIND_PACKAGE(izenelib REQUIRED COMPONENTS procmeminfo am izene_util)

IF(izenelib_FOUND)
    MESSAGE(STATUS "izenelib:")
    MESSAGE(STATUS "  header: ${izenelib_INCLUDE_DIRS}")
ELSE(izenelib_FOUND)
    MESSAGE(STATUS "izenelib: not found")
ENDIF(izenelib_FOUND)

SET( USE_IZENECMA TRUE )
SET( USE_WISEKMA TRUE)
SET( USE_IZENEJMA TRUE )

add_definitions("-fpermissive")
add_definitions(-DLANGID_DEBUG_PRINT=${CMAKE_LANGID_DEBUG_PRINT})

IF( USE_WISEKMA )
  FIND_PACKAGE(wisekma REQUIRED)
  ADD_DEFINITIONS( -DUSE_WISEKMA=TRUE )
ELSE( USE_WISEKMA )
  SET( wisekma_LIBRARIES "" )
  SET( wisekma_INCLUDE_DIRS "" )
  SET( wisekma_LIBRARY_DIRS "" )
ENDIF( USE_WISEKMA )

IF( USE_IZENECMA )
  FIND_PACKAGE(izenecma REQUIRED)
  ADD_DEFINITIONS( -DUSE_IZENECMA=TRUE )
ELSE( USE_IZENECMA )
  SET( izenecma_INCLUDE_DIRS "" )
  SET( izenecma_LIBRARIES "" )
  SET( izenecma_LIBRARY_DIRS "" )
ENDIF( USE_IZENECMA )

IF( USE_IZENEJMA )
  FIND_PACKAGE(izenejma REQUIRED)
  ADD_DEFINITIONS( -DUSE_IZENEJMA=TRUE )
ELSE( USE_IZENEJMA )
  SET( izenejma_INCLUDE_DIRS "" )
  SET( izenejma_LIBRARIES "" )
  SET( izenejma_LIBRARY_DIRS "" )
ENDIF( USE_IZENEJMA )

FIND_PACKAGE(TokyoCabinet 1.4.29 REQUIRED)
IF(TokyoCabinet_FOUND)
    MESSAGE(STATUS "TokyoCabine:")
    MESSAGE(STATUS "  header: ${TokyoCabinet_INCLUDE_DIRS}")
    MESSAGE(STATUS "  version: ${TokyoCabinet_VERSION}")
    include_directories(${TokyoCabinet_INCLUDE_DIRS})
ELSE(TokyoCabinet_FOUND)
    MESSAGE(STATUS "TokyoCabinet: not found")
ENDIF(TokyoCabinet_FOUND)

FIND_PACKAGE(Glog REQUIRED)
IF(Glog_FOUND)
    MESSAGE(STATUS "Google-glog:")
    MESSAGE(STATUS "  header: ${Glog_INCLUDE_DIRS}")
    include_directories(${Glog_INCLUDE_DIRS})
ELSE(Glog_FOUND)
    MESSAGE(STATUS "Google-glog: not found")
ENDIF(Glog_FOUND)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${izenelib_INCLUDE_DIRS}
  ${wisekma_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${izenecma_INCLUDE_DIRS}
  ${izenejma_INCLUDE_DIRS}
)

INSTALL(DIRECTORY include/ DESTINATION include)
INSTALL(DIRECTORY db/langid/ DESTINATION db/langid)


SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")

set(
LIBS
ilplib
${izenecma_LIBRARIES}
${wisekma_LIBRARIES}
${izenejma_LIBRARIES}
${izenelib_LIBRARIES}
${Boost_LIBRARIES}
${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
${Glog_LIBRARIES}
pthread
)

add_subdirectory(source)

##################################################
# CPack
#####
INCLUDE(UseCPack)
CONFIGURE_FILE(
  "ilplib-version.h.cmake"
  "${CMAKE_SOURCE_DIR}/include/ilplib-version.h"
  @ONLY
  )
SET(CPACK_DEBIAN_PACKAGE_DEPENDS
  "libboost-dev, libc6-dev, izenelib, icma")


LINK_DIRECTORIES(
    ${izenecma_LIBRARY_DIRS}
    ${wisekma_LIBRARY_DIRS}
    ${izenejma_LIBRARIES_DIRS}
    ${izenelib_LIBRARY_DIRS}
    )

