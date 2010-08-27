FILE( GLOB_RECURSE SOURCE
  "${CMAKE_CURRENT_SOURCE_DIR}/la/source/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/langid/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/question-answering/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/sbd/*.cpp"

  #"stem/*.cpp"
  #"wrapper/*.cpp"
  #"pos/*.cpp"
  )

ADD_LIBRARY(ilplib STATIC ${SOURCE})
INSTALL(TARGETS ilplib
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/langid)
ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/langid/test")
ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/langid/src")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/la)
ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/la/test")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/sbd)
ADD_SUBDIRECTORY("${CMAKE_CURRENT_SOURCE_DIR}/sbd/test")



#add_executable(langid_create_profile "${CMAKE_CURRENT_SOURCE_DIR}/langid/test/langid_create_profile.cpp")
#target_link_libraries(langid_create_profile ${LIBS})
#add_executable(langid_measure_profile langid_measure_profile.cpp)
#target_link_libraries(langid_measure_profile ${LIBS})
#add_executable(langid_select_profile langid_select_profile.cpp)
#target_link_libraries(langid_select_profile ${LIBS})
#add_executable(langid_build_encoding_model langid_build_encoding_model.cpp)
#target_link_libraries(langid_build_encoding_model ${LIBS})
#add_executable(langid_build_language_model langid_build_language_model.cpp)
#target_link_libraries(langid_build_language_model ${LIBS})
