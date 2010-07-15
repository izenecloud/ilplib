SET( LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/../lib" )

FIND_PACKAGE(izenelib REQUIRED COMPONENTS procmeminfo am izene_util)

IF(izenelib_FOUND)
    MESSAGE(STATUS "izenelib:")
    MESSAGE(STATUS "  header: ${izenelib_INCLUDE_DIRS}")
ELSE(izenelib_FOUND)
    MESSAGE(STATUS "izenelib: not found")
ENDIF(izenelib_FOUND)

IF(ENABLE_TEST)
  ENABLE_TESTING()
ENDIF(ENABLE_TEST)

SET( USE_IZENECMA TRUE )
SET( USE_WISEKMA TRUE)

add_definitions("-fpermissive")

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


INCLUDE_DIRECTORIES(
  ${izenelib_INCLUDE_DIRS}
  ${wisekma_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${izenecma_INCLUDE_DIRS}
  )

LINK_DIRECTORIES(
    ${izenecma_LIBRARY_DIRS}
    ${wisekma_LIBRARY_DIRS}
    ${izenelib_LIBRARY_DIRS}
    )

##################################################
# Messages
#####
IF(VERBOSE)

IF( USE_WISEKMA )
  IF(wisekma_FOUND)
    MESSAGE(STATUS "wisekma:")
    MESSAGE(STATUS "  header: ${wisekma_INCLUDE_DIRS}")
    MESSAGE(STATUS "  lib: ${wisekma_LIBRARIES}")
    MESSAGE(STATUS "  knowledge: ${wisekma_KNOWLEDGE}")
  ELSE(wisekma_FOUND)
    MESSAGE(STATUS "WISEKMA: not found")
  ENDIF(wisekma_FOUND)
ENDIF( USE_WISEKMA )


  IF( USE_IZENECMA )
    IF(izenecma_FOUND)
      MESSAGE(STATUS "izenecma:")
      MESSAGE(STATUS "  header: ${izenecma_INCLUDE_DIRS}")
    ELSE(izenecma_FOUND)
      MESSAGE(STATUS "IZENECMA: not found")
    ENDIF(izenecma_FOUND)
  ENDIF( USE_IZENECMA )

ENDIF(VERBOSE)

add_subdirectory(source)

#IF(ENABLE_TEST)
  ADD_SUBDIRECTORY(test)
#ENDIF(ENABLE_TEST)