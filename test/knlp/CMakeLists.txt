
INCLUDE_DIRECTORIES(
    ${IZENELIB_SOURCE_DIR}/include/
)


ADD_EXECUTABLE(tokenize_unigram_train 
			tokenize_unigram_train.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(tokenize_unigram_train
   ${Boost_SYSTEM_LIBRARY}
   pthread
)

ADD_EXECUTABLE(tokenize_util 
			tokenize_util.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(tokenize_util
   ${Boost_SYSTEM_LIBRARY}
)

ADD_EXECUTABLE(fill_idf 
			fill_idf.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(fill_idf
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
)
ADD_EXECUTABLE(fill_naive_bayes 
			fill_naive_bayes.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(fill_naive_bayes
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
)

ADD_EXECUTABLE(grep 
			grep.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(grep
   ${Boost_SYSTEM_LIBRARY}
)

ADD_EXECUTABLE(token_t-test 
			token_t-test.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(token_t-test
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
)

ADD_EXECUTABLE(etao_downloader 
			etao_downloader.cpp 
)

TARGET_LINK_LIBRARIES(etao_downloader
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   boost_regex
   curl
)

ADD_EXECUTABLE(taobao_downloader 
			taobao_downloader.cpp 
)

TARGET_LINK_LIBRARIES(taobao_downloader
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   boost_regex
   curl
)

ADD_EXECUTABLE(makeup_etao_downloader 
			makeup_etao_downloader.cpp 
)

TARGET_LINK_LIBRARIES(makeup_etao_downloader
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   boost_regex
   curl
)

ADD_EXECUTABLE(fmm_util 
			fmm_util.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(fmm_util
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
)

SET(libs
  ilplib
  ${Boost_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  ${izenelib_LIBRARIES}
  ${izenecma_LIBRARIES}
  ${izenejma_LIBRARIES}
  ${wisekma_LIBRARIES}
  ${Glog_LIBRARIES}
)

ADD_EXECUTABLE(t_knlp_tokenize
  t_knlp_tokenize.cpp
  KNlpTokenizeTestFixture.cpp
  Runner.cpp
)

TARGET_LINK_LIBRARIES(t_knlp_tokenize ${libs})

ADD_EXECUTABLE(tao_cate_test 
			tao_cate_test.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(tao_cate_test
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
)

