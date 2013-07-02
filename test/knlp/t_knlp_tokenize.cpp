/**
 * @file t_knlp_tokenize.cpp
 * @brief test ilplib::knlp::Tokenize in forward maximize match.
 */

#include "KNlpTokenizeTestFixture.h"
#include <boost/test/unit_test.hpp>

namespace
{
const char* kDictContent =
    "[MIN]	0.3\n"
    "苹	2.3\n"
    "苹果	10.4\n"
    "苹果手机	15.5\n"
    "iphone	20.7\n"
    "手机	12.1\n"
    "双模	13.1\n"
    "双模双待	14.1\n"
    "三星	10.8\n"
    "宽肩立领风衣外套	16.7\n"
    ;
}

BOOST_FIXTURE_TEST_SUITE(KNlpTokenizeTest, KNlpTokenizeTestFixture)

BOOST_AUTO_TEST_CASE(testFMM)
{
    init(kDictContent);

    // for token not exist in dictionary, if it contains no more than 3 Chinese
    // characters, then it would be assumed as one token
    checkFMM("APPLE/苹果 IPHONE (16GB)   电信版 手机",
             "apple / 苹果 iphone ( 16gb ) 电信版 手机");

    checkFMM("HTC T329d 3G手机(黑色)CDMA2000/GSM 【双模双待双通】",
             "htc t329d 3g 手机 ( 黑色 ) cdma2000 / gsm 【 双模双待 双 通 】");

    checkFMM("三星 S7562I 3G手机(纯白)WCDMA/GSM 双卡双待",
             "三星 s7562i 3g 手机 ( 纯白 ) wcdma / gsm 双 卡 双 待");

    checkFMM("艾米 AMII 宽肩立领风衣外套C1NC5121137",
             "艾米 amii 宽肩立领风衣外套 c1nc5121137");
}

BOOST_AUTO_TEST_SUITE_END()
