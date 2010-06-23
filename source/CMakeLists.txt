cmake_minimum_required(VERSION 2.6)

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
  SET(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
  SET(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline -DIZS_DEBUG" CACHE STRING "Debugfull build type")
  SET(CMAKE_CXX_FLAGS_PROFILE        "-pg -O2 -DNDEBUG")
  #SET(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs" CACHE STRING "Profile build type")

  MARK_AS_ADVANCED(CMAKE_CXX_FLAGS_DEBUGFULL CMAKE_CXX_FLAGS_PROFILE)
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

project(ILPLIB)

IF (CMAKE_COMPILER_IS_GNUCXX)
add_definitions("-Wall -Wno-deprecated")
ENDIF (CMAKE_COMPILER_IS_GNUCXX)

# Boost package
SET(Boost_ADDITIONAL_VERSIONS 1.39 1.39.0 1.38 1.38.0 1.37 1.37.0)
FIND_PACKAGE(Boost 1.36 COMPONENTS
    program_options thread serialization filesystem system)
FIND_PACKAGE(izenelib REQUIRED COMPONENTS procmeminfo am izene_util)
    
IF(Boost_FOUND)
    MESSAGE(STATUS "BOOST:")
    MESSAGE(STATUS "  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    MESSAGE(STATUS "  Boost_LIBRARIES: ${Boost_LIBRARIES}")
    MESSAGE(STATUS "  version: ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")
    include_directories(${Boost_INCLUDE_DIR})
ELSE(Boost_FOUND)
    MESSAGE(STATUS "BOOST: not found")
ENDIF(Boost_FOUND)

IF(izenelib_FOUND)
    MESSAGE(STATUS "izenelib:")
    MESSAGE(STATUS "  header: ${izenelib_INCLUDE_DIRS}")
ELSE(izenelib_FOUND)
    MESSAGE(STATUS "izenelib: not found")
ENDIF(izenelib_FOUND)

include_directories(../include)

add_subdirectory(langid)
add_subdirectory(la)
