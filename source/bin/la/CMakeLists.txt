SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/la)

set(la_libs
    ilplib
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${izenelib_LIBRARIES}
    ${izenecma_LIBRARIES}
    ${wisekma_LIBRARIES}
    ${Glog_LIBRARIES}
 )

  
add_executable(t_LACMD t_LACMD.cpp)

target_link_libraries(t_LACMD ${la_libs})