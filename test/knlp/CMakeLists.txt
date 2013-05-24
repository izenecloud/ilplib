
INCLUDE_DIRECTORIES(
    ${IZENELIB_SOURCE_DIR}/include/
)


ADD_EXECUTABLE(tokenize_unigram_train 
			tokenize_unigram_train.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(tokenize_unigram_train
   ${Boost_SYSTEM_LIBRARY}
)

ADD_EXECUTABLE(tokenize_util 
			tokenize_util.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(tokenize_util
   ${Boost_SYSTEM_LIBRARY}
)
