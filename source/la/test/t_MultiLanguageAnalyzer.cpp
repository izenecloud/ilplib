/**
 * @author Wei
 */

#include <boost/test/unit_test.hpp>
#include <util/ustring/UString.h>
#include <langid/langid.h>
/**
#include <la/analyzer/MultiLanguageAnalyzer.h>
#include <la/analyzer/ChineseAnalyzer.h>
#include <la/analyzer/KoreanAnalyzer.h>
#include <la/analyzer/EnglishAnalyzer.h>
*/

#include <ilplib.hpp>

#include "test_def.h"

using namespace la;
using namespace std;
using namespace izenelib::util;

BOOST_AUTO_TEST_SUITE( MultiLanguageAnalyzerTest )

BOOST_AUTO_TEST_CASE(test_detect_language)
{
    MultiLanguageAnalyzer analyzer;
    ilplib::langid::Factory* langIdFactory = ilplib::langid::Factory::instance();
    MultiLanguageAnalyzer::langIdAnalyzer_ = langIdFactory->createAnalyzer();
    ilplib::langid::Knowledge* langIdKnowledge_ = langIdFactory->createKnowledge();
    langIdKnowledge_->loadEncodingModel("../db/langid/model/encoding.bin");
    langIdKnowledge_->loadLanguageModel("../db/langid/model/language.bin");
    MultiLanguageAnalyzer::langIdAnalyzer_->setKnowledge(langIdKnowledge_);

    BOOST_CHECK_EQUAL( analyzer.detectLanguage(UString("互联网(the Internet)中国 2010。", UString::UTF_8)), MultiLanguageAnalyzer::CHINESE);
    BOOST_CHECK_EQUAL( analyzer.detectLanguage(UString("(the Internet) 中国 2010。", UString::UTF_8)), MultiLanguageAnalyzer::CHINESE);
    BOOST_CHECK_EQUAL( analyzer.detectLanguage(UString("윈도7 Windows7", UString::UTF_8)), MultiLanguageAnalyzer::KOREAN);
    BOOST_CHECK_EQUAL( analyzer.detectLanguage(UString("Windows7 윈도7", UString::UTF_8)), MultiLanguageAnalyzer::KOREAN);
    BOOST_CHECK_EQUAL( analyzer.detectLanguage(UString("This is boost unit test", UString::UTF_8)), MultiLanguageAnalyzer::ENGLISH);

    BOOST_CHECK_EQUAL( analyzer.detectLanguage(UString("LG카드 채권단, 5시부터 긴급 은행장 회의 25% 마감 dinasour 중산 민층", UString::UTF_8)), MultiLanguageAnalyzer::KOREAN);
    BOOST_CHECK_EQUAL( analyzer.detectLanguage(UString("◇조합예탁금 및 농어가목돈마련저축 비과세 3년연장(조특법)&lt;br&gt;&lt;pre&gt;&lt;font size=3&gt;\
        ┌─────────────────┬───────────────────┐&lt;br&gt;│            정  부  안            │            수  정  사  항            │&lt;br&gt;\
        ├─────────────────┼───────────────────┤&lt;br&gt;│o 조합예탁금 :                    │─┐                                  │&lt;br&gt;\
        │ - ''04>년 5%, ''05년 10% 과세       │  │ o 3년 연장                       │&lt;br&gt;│o 농어가목돈마련저축              │  │  - ''06.12.31까지 비과세\
        ", UString::UTF_8)), MultiLanguageAnalyzer::KOREAN);
    BOOST_CHECK_EQUAL( analyzer.detectLanguage(UString("&nbsp;                                                                         &nbsp; &nbsp; &nbsp; \
    &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;    &nbsp; &nbsp; &nbsp;&nbsp; 수입차 사후 보증\
 쿠폰, &quot;말 뿐&quot; &nbsp;&nbsp;&nbsp;2001-11-22 수입차는 중고로 구입한 경우 손해?? 큰 것같습니다. 고급 수입차의 장점으로 꼽히는 각종 사후보증 서비스가 중고차로 사면 무용지물이 된다고 >합니다. \
기동취재 2000, 홍순준 기자입니다. 36살 오 모씨는 2년된 중고 수입 승용차를 사면서 2백만원 어치의 쿠폰을 함께 넘겨 받았습니다. 엔진오일과 벨트 등 각종 소모품을 기간에 관계없이 새로 갈>아준다는 쿠폰입니다. \
하지만 수입차 서비스 센터를 찾은 오씨는 당황스러웠습니다. {오 모씨/중고 수입차 구매자} \"제가 처음에 차를 산게 아니라서 쿠폰을 쓸 수 없다고 하더라구요. 처음에 구입한 사람 \
만 가능하다고요. 마감 재료도 거지같은게....\" 3, 4년 전부터 판촉전에 나선 수입차 업체들은 BMW 2백만원 선, 벤츠 4백만원선 등 평균 2백만원 이상 어치의 사후 보증 서비스를 내세웠습니다. 필터와 부\
동액, 브레이크 라이닝까지 바꿔 준다는 각종 혜택들이 실제 수입차 판매실적에 큰 도움이 됐습니다. 모든 수입차 딜러들이 이런 쿠폰과 각종 사후보증 서비스로 소비자를 유혹하고 있지만, 주인이 한번이\
라도 바뀌면 이런 서비스의 운명은 끝나고 맙니다. {수입차 서비스 센터 직원} \"등록증 상의 명의를 이전하면 쿠폰을 사용할 수 없습니다. 차를 팔 때 약관사항입니다.\" 심지어 수입업체들은 이런 혜택들?? 차값에\
포함돼 있지 않은, 공짜 서비스라고 강조합니다. {김영식/BMW차장 } \"사후 보증서비스는 순수 저희 마케팅 비용에서 무상으로 해 드리는 서비스입니다. (고객 부담은 전혀 없는 건가요?) 그렇습>니다.\" \
하지만 쿠폰은 '유가증권'으로, 수입업체는 따로 돈을 받고 쿠폰을 팔기도 합니다. {수입차 딜러} \"쿠폰이 3, 4백만원 정도합니다. 안 받을 경우엔 상당하는 금액을 빼주고 있습니다. (차값에서 빼\
주실 수 있다?) 그렇습니다.\" 결국 차값에 쿠폰 값이 포함돼 있다는 얘기입니다. 따라서 중고차 값에도 사후 서비스 비용이 들어가 있습니다. {임기상/자동차 10년타기 시민운동연합 } \"\
차값에 포함된 건 누구나 아는 사실이고, 중고차를 사도 응당 소비자가 자신의 권리로 주장할 수 있는 겁니다.\" 올 한해 동안만도 국내에서 7천대 이상의 수입 자동차가 팔릴 것으로 예상됩니다. \
하지만 허울만 좋은 사후 >서비스는 고스란히 수입업체의 수익만 키우고 있습니다. 홍순준 기자         SBS 문서 전체  이미지  뉴스  VOD  AOD                                   \
무용지물 된 중고 수입차 쿠폰  &nbsp;&nbsp; 옵션 첨부가 가능하다.", UString::UTF_8)), MultiLanguageAnalyzer::KOREAN);

    BOOST_CHECK_EQUAL( analyzer.detectLanguage(UString("人们总会不自觉地停下匆匆脚步，回看过去，盘点成败得失。对中医药界而言，２００７年无疑是波澜壮阔、加速度前进的一年。\
这一年里，史无前例的“中医中药中国行”大型科普宣传活动顺利启航，先进>模范人物不断涌现，新规范、新举措接连出台，不和谐的杂音日趋微弱……过去的已成为历史，让我们一同铭记。盘点要事，见证风云，开拓进取，再展新篇。\
２００７年党中央国务院对中医药的关怀１．吴仪出席２００７年全国中医药工作会议１月１１日，全国中医药工作会议在北京召开，会议总结了２００６年的中医药工作，对“十一五”期间中医药工作作了全面部署。\
吴仪副总理出席了这次会议，并发表重要讲话。她代表党中央、国务院进一步表明要“坚定不移地发展中医药事业”的态度。６月４日，《求是》杂志全文刊发了吴仪副总理的重要讲话———“推进继承创新发挥特色优势坚定不移地发展中医药事业”。\
吴仪副总理的重要讲话，为中>医药事业发展进一步指明了前进方向。２．温家宝在政府工作报告中强调“大力扶持中医药和民族医药发展”３月５日，温家宝总理在十届全国人大五次会议上所作的政府工作报告中又进一步强调，\
要“大力扶持中医药>和民族医药发展，充分发挥祖国传统医药在防病治病中的重要作用”。全国中医药界备受鼓舞和鞭策。３．吴仪为全军中医药技术大比武的“国医名师”颁奖并发表重要讲话８月３日，吴仪副总理出席全军中医药技术大>比武总决赛，\
为获奖选手颁奖并发表了重要讲话，更加有力地推动了我国中医药事业和军队中医药工作的开展。９月４日，《人民日报》全文刊发了吴仪副总理在全军中医药技术大比武总决赛上的重要讲话———“坚定不 \
移地大力扶持和发展中医药事业”。４．坚持“中西医并重”和“扶持中医药和民族医药事业发展”写入党的十七大报告１０月１５日，党的十七大通过的胡锦涛总书记所作的报告，把坚持“中西医并重”和“扶持中医药和民 \
族医药事业发展”写入党的政治宣言和行动纲领，又一次充分肯定了中医药和民族医药在我国全面建设小康社会、维护广大人民群众健康中的地位和作用，这是党中央一贯高度重视和关心支持中医药和民族医药事业发>展的具体体现。", UString::UTF_8)), MultiLanguageAnalyzer::CHINESE);
    BOOST_CHECK_EQUAL( analyzer.detectLanguage(UString("日本市场推出了它的超轻薄ＡｑｕｏｓＸ系列液晶电视，公司宣布日期称，它的超轻薄系列液晶电视将在今年九月份投放欧洲市场。", UString::UTF_8)), MultiLanguageAnalyzer::CHINESE);

}


BOOST_AUTO_TEST_CASE(test_normal)
{
    boost::shared_ptr<Analyzer> pChineseAnalyzer(new ChineseAnalyzer(CMA_KNOWLEDGE));
    boost::static_pointer_cast<ChineseAnalyzer, Analyzer>(pChineseAnalyzer)->setLabelMode();
    boost::static_pointer_cast<ChineseAnalyzer, Analyzer>(pChineseAnalyzer)->setAnalysisType(ChineseAnalyzer::minimum_match);

    boost::shared_ptr<Analyzer> pKoreanAnalyzer(new KoreanAnalyzer(KMA_KNOWLEDGE));
    boost::static_pointer_cast<KoreanAnalyzer, Analyzer>(pKoreanAnalyzer)->setLabelMode();

    boost::shared_ptr<Analyzer> pEnglishAnalyzer(new EnglishAnalyzer());
    boost::static_pointer_cast<EnglishAnalyzer, Analyzer>(pEnglishAnalyzer)->setCaseSensitive(true, false);

    MultiLanguageAnalyzer analyzer;
    analyzer.setAnalyzer(MultiLanguageAnalyzer::CHINESE, pChineseAnalyzer );
    analyzer.setAnalyzer(MultiLanguageAnalyzer::KOREAN,  pKoreanAnalyzer );
    analyzer.setAnalyzer(MultiLanguageAnalyzer::ENGLISH, pEnglishAnalyzer );
    {
        TermList termList;
        analyzer.analyze(UString("测试使用Boost Unit Tests", UString::UTF_8), termList);
        BOOST_CHECK_EQUAL(termList.size(), 5U);
        BOOST_CHECK_EQUAL( termList[0].text_, UString("测试", UString::UTF_8) );
        BOOST_CHECK_EQUAL( termList[1].text_, UString("使用", UString::UTF_8) );
        BOOST_CHECK_EQUAL( termList[2].text_, UString("boost", UString::UTF_8) );
        BOOST_CHECK_EQUAL( termList[3].text_, UString("unit", UString::UTF_8) );
        BOOST_CHECK_EQUAL( termList[4].text_, UString("tests", UString::UTF_8) );
    }
    {
        TermList termList;
        analyzer.analyze(UString("멀티터치스크린 기술인", UString::UTF_8), termList);
        BOOST_CHECK_EQUAL(termList.size(), 6U);
        BOOST_CHECK_EQUAL( termList[0].text_, UString("멀티터치스크린", UString::UTF_8) );
        BOOST_CHECK_EQUAL( termList[0].wordOffset_, 0U);
        BOOST_CHECK_EQUAL( termList[1].text_, UString("멀티", UString::UTF_8) );
        BOOST_CHECK_EQUAL( termList[1].wordOffset_, 0U);
        BOOST_CHECK_EQUAL( termList[2].text_, UString("터치", UString::UTF_8) );
        BOOST_CHECK_EQUAL( termList[2].wordOffset_, 0U);
        BOOST_CHECK_EQUAL( termList[3].text_, UString("스크린", UString::UTF_8) );
        BOOST_CHECK_EQUAL( termList[3].wordOffset_, 0U);
        BOOST_CHECK_EQUAL( termList[4].text_, UString("기술인", UString::UTF_8) );
        BOOST_CHECK_EQUAL( termList[4].wordOffset_, 1U);
        BOOST_CHECK_EQUAL( termList[5].text_, UString("기술", UString::UTF_8) );
        BOOST_CHECK_EQUAL( termList[5].wordOffset_, 1U);
    }
    {
        TermList termList;
        analyzer.analyze(UString("Boost unit tests.", UString::UTF_8), termList);
        BOOST_CHECK_EQUAL(termList.size(), 4U);
        BOOST_CHECK_EQUAL(termList.size(), 4U);
        BOOST_CHECK_EQUAL(termList[0].text_, UString("Boost", UString::UTF_8));
        BOOST_CHECK_EQUAL(termList[0].wordOffset_, 0U);
        BOOST_CHECK_EQUAL(termList[1].text_, UString("unit", UString::UTF_8));
        BOOST_CHECK_EQUAL(termList[1].wordOffset_, 1U);
        BOOST_CHECK_EQUAL(termList[2].text_, UString("tests", UString::UTF_8));
        BOOST_CHECK_EQUAL(termList[2].wordOffset_, 2U);
        BOOST_CHECK_EQUAL(termList[3].text_, PLACE_HOLDER);
        BOOST_CHECK_EQUAL(termList[3].wordOffset_, 3U);
    }
}

BOOST_AUTO_TEST_SUITE_END()
