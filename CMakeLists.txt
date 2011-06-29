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
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -fPIC")
  # Disable assertion macros in RELEASE
  SET(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG -fPIC")
  #SET(CMAKE_CXX_FLAGS_RELEASE        "-O2 -g")
  SET(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline -fPIC")
  SET(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline -DIZS_DEBUG" -fPIC CACHE STRING "Debugfull build type")
  SET(CMAKE_CXX_FLAGS_PROFILE        "-pg -O2 -DNDEBUG -fPIC")
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
  source/langid/include
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

#IF( USE_LANGID )
#    add_subdirectory(langid)
#ENDIF( USE_LANGID )
IF( USE_LA )
    add_subdirectory(source)
ENDIF( USE_LA )
#IF( USE_QA )
#    add_subdirectory(question-answering)
#ENDIF( USE_QA )


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

