FILE( GLOB_RECURSE SOURCE
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  #"stem/*.cpp"
  #"wrapper/*.cpp"
  #"pos/*.cpp"
  )

ADD_LIBRARY(la SHARED ${SOURCE})
