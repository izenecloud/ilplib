# objs
set(objs_langid
./factory
./analyzer
./analyzer_impl
./knowledge
./knowledge_impl
./profile
./profile_selector
./language_analyzer
./script_table
./sentence_break_table
./sentence_tokenizer
./profile_model
./encoding_analyzer
)

add_library(langid STATIC ${objs_langid})
INSTALL(TARGETS langid
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

add_executable(langid_create_profile langid_create_profile.cpp)
target_link_libraries(langid_create_profile ${LIBS_LANGID} ${LIBS_BOOST})
add_executable(langid_measure_profile langid_measure_profile.cpp)
target_link_libraries(langid_measure_profile ${LIBS_LANGID} ${LIBS_BOOST})
add_executable(langid_select_profile langid_select_profile.cpp)
target_link_libraries(langid_select_profile ${LIBS_LANGID} ${LIBS_BOOST})
add_executable(langid_build_encoding_model langid_build_encoding_model.cpp)
target_link_libraries(langid_build_encoding_model ${LIBS_LANGID} ${LIBS_BOOST})
add_executable(langid_build_language_model langid_build_language_model.cpp)
target_link_libraries(langid_build_language_model ${LIBS_LANGID})
