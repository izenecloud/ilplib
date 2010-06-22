##################################################
# Boost
#####
SET(Boost_ADDITIONAL_VERSIONS 1.40 1.40.0 1.39 1.39.0 1.38 1.38.0 1.37 1.37.0)
FIND_PACKAGE(Boost 1.36 REQUIRED COMPONENTS
  thread serialization filesystem system)

##################################################
# Threads
#####
FIND_PACKAGE(Threads REQUIRED)

##################################################
# izenelib wiselib kma
#####
FIND_PACKAGE(izenelib REQUIRED COMPONENTS procmeminfo am)
FIND_PACKAGE(wiselib REQUIRED)

IF( USE_WISEKMA )
  FIND_PACKAGE(wisekma REQUIRED)
  ADD_DEFINITIONS( -DUSE_WISEKMA=TRUE )
ELSE( USE_WISEKMA )
  SET( wisekma_LIBRARIES "" )
  SET( wisekma_INCLUDE_DIRS "" )
  SET( wisekma_LIBRARY_DIRS "" )
ENDIF( USE_WISEKMA )

##################################################
# whether use izenecma
#####
OPTION(USE_IZENECMA "use izenecma?" OFF)
IF( USE_IZENECMA )
  ADD_DEFINITIONS( -DUSE_IZENECMA=TRUE )
  SET( LIB_CMAC "cmac")
ELSE( USE_IZENECMA )
  #set library empty
  SET( LIB_CMAC "")
  SET( izenecma_INCLUDE_DIRS "" )
  SET( izenecma_LIBRARY_DIRS "" )
ENDIF( USE_IZENECMA )
MARK_AS_ADVANCED(USE_IZENECMA)

IF( USE_IZENECMA )
  SET(izenecma_FOUND FALSE)
  IF(IS_DIRECTORY "$ENV{IZENECMA}"
      AND IS_DIRECTORY "$ENV{IZENECMA}/include"
      AND IS_DIRECTORY "$ENV{IZENECMA}/db/icwb/utf8")
    SET(izenecma_FOUND TRUE)
    SET(izenecma_INCLUDE_DIRS "$ENV{IZENECMA}/include")
    SET(izenecma_LIBRARY_DIRS "$ENV{IZENECMA}/lib")
    SET(izenecma_KNOWLEDGE_DIRS "$ENV{IZENECMA}/db/icwb/utf8")
  ENDIF(IS_DIRECTORY "$ENV{IZENECMA}"
    AND IS_DIRECTORY "$ENV{IZENECMA}/include"
    AND IS_DIRECTORY "$ENV{IZENECMA}/db/icwb/utf8")

  IF(NOT izenecma_FOUND)
    MESSAGE(FATAL_ERROR "cannot found izenecma, please set env variable IZENECMA")
  ENDIF(NOT izenecma_FOUND)
ENDIF( USE_IZENECMA )
