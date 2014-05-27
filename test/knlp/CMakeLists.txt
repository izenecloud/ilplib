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

ADD_EXECUTABLE(horse_tokenize_util 
			horse_tokenize_test.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(horse_tokenize_util
   ${Boost_SYSTEM_LIBRARY}
)

ADD_EXECUTABLE(title_pca_util 
			title_pca_util.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(title_pca_util
   ${LIBS}
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
   ${Boost_REGEX_LIBRARY}
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
   ${Boost_REGEX_LIBRARY}
   curl
)

ADD_EXECUTABLE(taobao_downloader 
			taobao_downloader.cpp 
)

TARGET_LINK_LIBRARIES(taobao_downloader
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   ${Boost_REGEX_LIBRARY}
   curl
)

ADD_EXECUTABLE(makeup_etao_downloader 
			makeup_etao_downloader.cpp 
)

TARGET_LINK_LIBRARIES(makeup_etao_downloader
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   ${Boost_REGEX_LIBRARY}
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


ADD_EXECUTABLE(t_kdictionary
  t_kdictionary.cpp
  Runner.cpp
)

TARGET_LINK_LIBRARIES(t_kdictionary ${libs})


ADD_EXECUTABLE(t_datrie
  t_datrie.cpp
  Runner.cpp
)

TARGET_LINK_LIBRARIES(t_datrie ${libs})

ADD_EXECUTABLE(tao_cate_test 
			tao_cate_test.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(tao_cate_test
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
)

ADD_EXECUTABLE(william_trie_util
			william_trie_util.cpp 
            ${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(william_trie_util
   ${Boost_SYSTEM_LIBRARY}
)

ADD_EXECUTABLE(cate_precision
			cate_precision.cpp 
            ${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(cate_precision
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_REGEX_LIBRARY}
)

ADD_EXECUTABLE(normaize_util
			normaize_util.cpp 
            ${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(normaize_util
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_REGEX_LIBRARY}
)

ADD_EXECUTABLE(new_tao_cate_test 
    new_tao_cate_test.cpp 
${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
            )

TARGET_LINK_LIBRARIES(new_tao_cate_test
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
)


ADD_EXECUTABLE(datrie_util
			datrie_util.cpp 
            ${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(datrie_util
   ${Boost_SYSTEM_LIBRARY}
)

ADD_EXECUTABLE(t_load_resource
  t_load_resource.cpp
  Runner.cpp
)

TARGET_LINK_LIBRARIES(t_load_resource ${libs})

ADD_EXECUTABLE(maxent_util 
    maxent_util.cpp 
    ${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
${PROJECT_SOURCE_DIR}/source/knlp/maxent/maxent.cpp
${PROJECT_SOURCE_DIR}/source/knlp/maxent/lbfgs.cpp
${PROJECT_SOURCE_DIR}/source/knlp/maxent/owlqn.cpp
${PROJECT_SOURCE_DIR}/source/knlp/maxent/sgd.cpp
            )

TARGET_LINK_LIBRARIES(maxent_util
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   ${Boost_REGEX_LIBRARY}
)

ADD_EXECUTABLE(kw_condition_util
			kw_condition_util.cpp 
)
TARGET_LINK_LIBRARIES(kw_condition_util
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_REGEX_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   ${libs}
   
)

ADD_EXECUTABLE(attr_normalize_util
			attr_normalize_util.cpp 
)

TARGET_LINK_LIBRARIES(attr_normalize_util
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_REGEX_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   ${libs}
   
)
ADD_EXECUTABLE(attr_token_util
			attr_token_util.cpp 
)

TARGET_LINK_LIBRARIES(attr_token_util
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_REGEX_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   ${libs}
   
)

ADD_EXECUTABLE(cluster_util
		cluster_util.cpp 
)

TARGET_LINK_LIBRARIES(cluster_util
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_REGEX_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   ${libs}
)


ADD_EXECUTABLE(get_tags_util 
			get_tags_util.cpp 
			${PROJECT_SOURCE_DIR}/source/knlp/normalize.cpp
)

TARGET_LINK_LIBRARIES(get_tags_util
   ${Boost_SYSTEM_LIBRARY}
)


