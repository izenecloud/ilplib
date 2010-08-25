cmake_minimum_required(VERSION 2.6)

project(ILPLIB)

# Check the mode
IF( NOT MODE )
    MESSAGE( STATUS "Set Default Mode to ALL")
    SET( MODE "ALL")
ENDIF( NOT MODE )
STRING(TOUPPER ${MODE} UPPER_MODE)
IF( ${UPPER_MODE} STREQUAL "ALL" )
    SET( USE_LANGID TRUE )
    SET( USE_LA TRUE )
    SET( USE_QA TRUE )
    MESSAGE( STATUS "ALL mode: include langid, la and qa.")
ELSEIF( ${UPPER_MODE} STREQUAL "LANGID" )
    SET( USE_LANGID TRUE )
    SET( USE_LA FALSE )
    SET( USE_QA FALSE )
    MESSAGE( STATUS "LANGID mode: include langid.")
ELSE( ${UPPER_MODE} STREQUAL "ALL" )
    MESSAGE( FATAL_ERROR "Invalid parameter mode: ${MODE}, should be one of LANGID or ALL")
ENDIF( ${UPPER_MODE} STREQUAL "ALL" )

#set(CMAKE_VERBOSE_MAKEFILE on)

# In CMake, compiler flags can be grouped in Configurations
#

# - Set default configurations definition, also add configuration Debugfull and Profile.
#
# Debugfull adds more debug information, and Profile adds profiling information as well.
#

IF (NOT CMAKE_BUILD_TYPE)
  #SET(CMAKE_BUILD_TYPE RelWithDebInfo)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF (NOT CMAKE_BUILD_TYPE)

MESSAGE(STATUS "CMAKE_COMPILER_IS_GNUCXX: " ${CMAKE_COMPILER_IS_GNUCXX})
IF (CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} "Debugfull" "Profile")

  # Select flags.
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
  # Disable assertion macros in RELEASE
  SET(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG")
  #SET(CMAKE_CXX_FLAGS_RELEASE        "-O2 -g")
  SET(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
  SET(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline -DIZS_DEBUG" CACHE STRING "Debugfull build type")
  SET(CMAKE_CXX_FLAGS_PROFILE        "-pg -O2 -DNDEBUG")
  #SET(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs" CACHE STRING "Profile build type")

  MARK_AS_ADVANCED(CMAKE_CXX_FLAGS_DEBUGFULL CMAKE_CXX_FLAGS_PROFILE)
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

IF( USE_LA OR USE_QA )
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
ENDIF( USE_LA OR USE_QA )

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

IF( USE_LA OR USE_QA )
FIND_PACKAGE(izenelib REQUIRED COMPONENTS procmeminfo am izene_util)

IF(izenelib_FOUND)
    MESSAGE(STATUS "izenelib:")
    MESSAGE(STATUS "  header: ${izenelib_INCLUDE_DIRS}")
ELSE(izenelib_FOUND)
    MESSAGE(STATUS "izenelib: not found")
ENDIF(izenelib_FOUND)
ENDIF( USE_LA OR USE_QA )

include_directories(../include)
INSTALL(DIRECTORY ../include/ DESTINATION include)

IF( USE_LANGID )
    add_subdirectory(langid)
ENDIF( USE_LANGID )
IF( USE_LA )
    add_subdirectory(la)
ENDIF( USE_LA )
IF( USE_QA )
    add_subdirectory(question-answering)
ENDIF( USE_QA )


##################################################
# CPack
#####
INCLUDE(UseCPack)
CONFIGURE_FILE(
  "ilplib-version.h.cmake"
  "${CMAKE_SOURCE_DIR}/../include/ilplib-version.h"
  @ONLY
  )
SET(CPACK_DEBIAN_PACKAGE_DEPENDS
  "libboost-dev, libc6-dev, izenelib, icma")
