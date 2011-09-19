SET(Boost_USE_STATIC_LIBS OFF)
FIND_PACKAGE(Boost ${Boost_FIND_VERSION}
  COMPONENTS unit_test_framework)

IF(Boost_FOUND AND Boost_UNIT_TEST_FRAMEWORK_FOUND)
  INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
  )

  set(libs
    ilplib
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${izenelib_LIBRARIES}
    ${izenecma_LIBRARIES}
    ${izenejma_LIBRARIES}	
    ${wisekma_LIBRARIES}
    ${Glog_LIBRARIES}
 )

  add_executable(t_identify_primary_language
    Runner.cpp
    t_identify_primary_language.cpp
  )
  target_link_libraries(t_identify_primary_language ${libs})

  add_executable(t_identify_multi_language
    Runner.cpp
    t_identify_multi_language.cpp
  )
  target_link_libraries(t_identify_multi_language ${libs})

  add_executable(t_tokenize_sentence
    Runner.cpp
    t_tokenize_sentence.cpp
  )
  target_link_libraries(t_tokenize_sentence ${libs})

  add_executable(t_identify_encoding
    Runner.cpp
    t_identify_encoding.cpp
  )
  target_link_libraries(t_identify_encoding ${libs})

ENDIF(Boost_FOUND AND Boost_UNIT_TEST_FRAMEWORK_FOUND)
