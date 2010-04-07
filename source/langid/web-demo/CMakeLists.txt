add_executable(demo-server demo-server.cpp appws.c)
target_link_libraries(demo-server ${LIBS_LANGID} pthread)
