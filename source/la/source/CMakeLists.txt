INCLUDE_DIRECTORIES( 
    ${lalib_SOURCE_DIR}/include/stem
    )

FILE( GLOB_RECURSE SOURCE
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  #"stem/*.cpp"
  #"wrapper/*.cpp"
  #"pos/*.cpp"
  )

ADD_LIBRARY(la ${SOURCE})
TARGET_LINK_LIBRARIES(la ${wisekma_LIBRARIES} ${LIB_CMAC} ${izenelib_am_LIBRARY})

INSTALL(TARGETS la 
    LIBRARY DESTINATION lib 
    COMPONENT lalib_libraries)
