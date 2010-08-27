add_executable(test_ucs2_converter test_ucs2_converter.cpp)
target_link_libraries(test_ucs2_converter ${LIBS})

add_executable(test_script_table test_script_table.cpp)
target_link_libraries(test_script_table ${LIBS})

add_executable(test_sentence_break_table test_sentence_break_table.cpp)
target_link_libraries(test_sentence_break_table ${LIBS})

add_executable(test_sentence_tokenizer test_sentence_tokenizer.cpp)
target_link_libraries(test_sentence_tokenizer ${LIBS})

add_executable(test_language_analyzer test_language_analyzer.cpp)
target_link_libraries(test_language_analyzer ${LIBS})

add_executable(test_langid_run test_langid_run.cpp)
target_link_libraries(test_langid_run ${LIBS})

add_executable(test_multithread test_multithread.cpp)
target_link_libraries(test_multithread ${LIBS})
