#IF( USE_WISEKMA )
#    ADD_DEFINITIONS( -DWISEKMA=$(WISEKMA) )
#ENDIF( USE_WISEKMA )
#
#IF( USE_IZENECMA )
#    ADD_DEFINITIONS( -DIZENECMA=$(IZENECMA) )
#ENDIF( USE_IZENECMA )
#
#SET (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)
#
#INCLUDE_DIRECTORIES(".")
#
#SET(Boost_USE_STATIC_LIBS OFF)
#FIND_PACKAGE(Boost ${Boost_FIND_VERSION}
#  COMPONENTS unit_test_framework)
#
#IF(Boost_FOUND AND Boost_UNIT_TEST_FRAMEWORK_FOUND)
#  INCLUDE_DIRECTORIES(
#    ${Boost_INCLUDE_DIRS}
#  )

  add_executable(t_LA
	Runner.cpp
	t_Tokenizer.cpp
	t_CharAnalyzer.cpp
	t_EnglishAnalyzer.cpp
	t_ChineseAnalyzer.cpp
	t_KoreanAnalyzer.cpp
	t_MultiLanguageAnalyzer.cpp
	)

#  set(libs
#	la
#	${Boost_THREAD_LIBRARY}
#	${Boost_SYSTEM_LIBRARY}
#	${Boost_FILESYSTEM_LIBRARY}
#	${Boost_SERIALIZATION_LIBRARY}
#	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
#	${izenelib_LIBRARIES}
#	${izenecma_LIBRARIES}
#	${wisekma_LIBRARIES}
# )

  target_link_libraries(t_LA ${LIBS})
#ENDIF(Boost_FOUND AND Boost_UNIT_TEST_FRAMEWORK_FOUND)

