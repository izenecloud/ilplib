add_executable(test_ucs2_converter test_ucs2_converter.cpp)
target_link_libraries(test_ucs2_converter ${LIBS_LANGID})

add_executable(test_script_table test_script_table.cpp)
target_link_libraries(test_script_table ${LIBS_LANGID})

add_executable(test_sentence_break_table test_sentence_break_table.cpp)
target_link_libraries(test_sentence_break_table ${LIBS_LANGID})

add_executable(test_sentence_tokenizer test_sentence_tokenizer.cpp)
target_link_libraries(test_sentence_tokenizer ${LIBS_LANGID})

add_executable(test_language_analyzer test_language_analyzer.cpp)
target_link_libraries(test_language_analyzer ${LIBS_LANGID} ${LIBS_BOOST})

add_executable(test_langid_run test_langid_run.cpp)
target_link_libraries(test_langid_run ${LIBS_LANGID} ${LIBS_BOOST})

add_executable(test_multithread test_multithread.cpp)
target_link_libraries(test_multithread ${LIBS_LANGID} pthread)
