IF( USE_WISEKMA )
    ADD_DEFINITIONS( -DWISEKMA=$(WISEKMA) )
ENDIF( USE_WISEKMA )

IF( USE_IZENECMA )
    ADD_DEFINITIONS( -DIZENECMA=$(IZENECMA) )
ENDIF( USE_IZENECMA )

LINK_DIRECTORIES (${izenecma_LIBRARY_DIRS} ${wisekma_LIBRARY_DIRS})

INCLUDE_DIRECTORIES( 
    ${lalib_SOURCE_DIR}/include/stem
    )

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(t_Tokenizer t_Tokenizer.cpp)
add_executable(t_StemAnalyzer t_StemAnalyzer.cpp)
add_executable(t_LA t_LA.cpp)
add_executable(performance-test performance-test.cpp) 
add_executable(t_DynamicDict t_DynamicDict.cpp) 

SET_TARGET_PROPERTIES(
    performance-test

    PROPERTIES
	COMPILE_DEFINITIONS "SF1_TIME_CHECK"
	)

target_link_libraries( t_StemAnalyzer
	la
	${wiselib_LIBRARIES}
	${izenelib_procmeminfo_LIBRARY}
	${wisekma_LIBRARIES}
	${Boost_THREAD_LIBRARY}
	)

target_link_libraries(t_Tokenizer
	la
	${wiselib_LIBRARIES}
	${izenelib_procmeminfo_LIBRARY}
	${wisekma_LIBRARIES}
	${Boost_THREAD_LIBRARY}
	) 

target_link_libraries( t_LA
	la
	${wiselib_LIBRARIES}
	${izenelib_procmeminfo_LIBRARY}
	${wisekma_LIBRARIES}
	${Boost_THREAD_LIBRARY}
	${Boost_SERIALIZATION_LIBRARY}
	)

target_link_libraries( performance-test
	la
	${wisekma_LIBRARIES}
	${wiselib_LIBRARIES}
	${izenelib_procmeminfo_LIBRARY}
	${Boost_THREAD_LIBRARY}
	)

target_link_libraries( t_DynamicDict
	la
	${wiselib_LIBRARIES}
	${izenelib_procmeminfo_LIBRARY}
	${wisekma_LIBRARIES}
	${Boost_THREAD_LIBRARY}
	${Boost_SERIALIZATION_LIBRARY}
	)

