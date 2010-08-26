include_directories(./include)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

set(LIBS_LANGID langid)
set(LIBS_BOOST ${Boost_LIBRARIES})

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(web-demo)
