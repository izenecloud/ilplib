include_directories(./include)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)

set(LIBS_LANGID langid)
set(LIBS_BOOST ${Boost_LIBRARIES})

# set compile flag for whether to print debug information
add_definitions(-DLANGID_DEBUG_PRINT=${CMAKE_LANGID_DEBUG_PRINT})

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(web-demo)
