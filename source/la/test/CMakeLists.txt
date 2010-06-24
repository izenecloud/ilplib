IF( USE_WISEKMA )
    ADD_DEFINITIONS( -DWISEKMA=$(WISEKMA) )
ENDIF( USE_WISEKMA )

IF( USE_IZENECMA )
    ADD_DEFINITIONS( -DIZENECMA=$(IZENECMA) )
ENDIF( USE_IZENECMA )

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin/la)

include_directories(".")

add_executable(t_Tokenizer t_Tokenizer.cpp)
add_executable(t_StemAnalyzer t_StemAnalyzer.cpp)
add_executable(t_LA t_LA.cpp)
add_executable(performance-test performance-test.cpp) 
add_executable(t_DynamicDict t_DynamicDict.cpp) 

set(libs
	la
	${izenelib_LIBRARIES}
	${Boost_THREAD_LIBRARY}
	${Boost_SERIALIZATION_LIBRARY}
    ${izenecma_LIBRARIES}
    ${wisekma_LIBRARIES}
)

SET_TARGET_PROPERTIES(
    performance-test

    PROPERTIES
	COMPILE_DEFINITIONS "SF1_TIME_CHECK"
	)

target_link_libraries( t_StemAnalyzer
	${libs}
	)

target_link_libraries(t_Tokenizer
	${libs}
	) 

target_link_libraries( t_LA
	${libs}
	)

target_link_libraries( performance-test
	${libs}
	)

target_link_libraries( t_DynamicDict
	${libs}
	)

