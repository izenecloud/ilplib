#include <la/analyzer/ChineseAnalyzer.h>


// Foreign Language is Chinese POS
#define CHINESE_FL "NX"

// Special character is Chinese POS
#define CHINESE_SC "W"

// compound noun in Chinese POS
#define CHINESE_NNP "N"

// Person's name in Chinese POS
#define CHINESE_NNI "NR"

// Number in Chinese POS
#define CHINESE_SN "M"

namespace la
{

ChineseAnalyzer::ChineseAnalyzer( const std::string knowledgePath, bool loadModel = true )
    : CommonLanguageAnalyzer(knowledgePath, loadModel),
      pA_(NULL),
      pS_(NULL)
{
    cma::CMA_Factory* factory = cma::CMA_Factory::instance();

    pA_ = factory->createAnalyzer();

    cma::Knowledge* pK = 0;
    if( loadModel )
        pK = la::CMAKnowledge::getInstance(knowledgePath.c_str()).pKnowledge_;
    else
        pK = la::CMANoModelKnowledge::getInstance(knowledgePath.c_str()).pKnowledge_;

    pA_->setKnowledge( pK );

    pS_ = new cma::Sentence();

    flMorp_ = pA_->getCodeFromStr( CHINESE_FL );

    scMorp_ = pA_->getCodeFromStr( CHINESE_SC );

    encode_ = UString::UTF_8;

    setCaseSensitive(false);
    //    bSpecialChars_ = false;

    setIndexMode(); // Index mode is set by default
}

ChineseAnalyzer::~ChineseAnalyzer()
{
    delete pA_;
    delete pS_;
}

void ChineseAnalyzer::setIndexMode()
{
    if( pA_ == NULL )
    {
        throw std::logic_error( "ChineseAnalyzer::setIndexMode() is call with pA_ NULL" );
    }
    resetAnalyzer();

    //pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, 1 );

    vector<string> posList;
    addDefaultPOSList(posList);

    // unknown
    posList.push_back("un");

    pA_->setIndexPOSList(posList);
}

void ChineseAnalyzer::setLabelMode()
{
    if( pA_ == NULL )
    {
        throw std::logic_error( "ChineseAnalyzer::setLabelMode() is call with pA_ NULL" );
    }
    resetAnalyzer();

    //pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, 1 );

    // to index all the POS
    pA_->resetIndexPOSList( true );
}

void ChineseAnalyzer::resetAnalyzer()
{
    //pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, 1 );
    pA_->resetIndexPOSList( false );
}

void ChineseAnalyzer::addDefaultPOSList( vector<string>& posList )
{
    // defaulted index POS
    //This POS list should be keeped even if something are commented, hence which one
    // is default or not is clear.
    string defaultPOS[] =
    {
        "A",  //adjective
        "AD", //Adjective-like adverb
        //"AG", //形语素形容词性语素。形容词代码为a，语素代码ｇ前面置以A
        "AN",  //名形词具有名词功能的形容词。形容词代码a和名词代码n并在一起
        "B",  //区别词:大型一次性短效,取汉字'别'的声母
        //"C",  //conjunction
        //"D",  //adverb
        //"DG",  //副语素副词性语素。副词代码为d，语素代码ｇ前面置以D
        //"E",  //exclamation
        //"F",  //Position, like 'up', 'down'
        //"G",  //语素绝大多数语素都能作为合成词的'词根'，取汉字'根'的声母
        //"H",  //head
        "I",  //Idiom
        "J",  //Abbreviation
        "K",  //appending part
        "L",  //Temporarily used words
        //"M",  //numeral
        //"MQ",  //number and unit
        //"NG",  //名语素名词性语素。名词代码为n，语素代码ｇ前面置以N
        "N",  //noun
        "NR",  //Person's Name
        "NS",  //Space's(Location) Name
        "NT",  //Organization's Name
        CHINESE_FL,  //English words
        "NZ",  //Proper Nouns
        //"O",  //onomatopoeia
        //"P",  //prepositional
        //"Q",  //quantity
        //"R",  //pronoun
        "S",  //Space or Location
        //"TG", // 时语素时间词性语素。时间词代码为t,在语素的代码g前面置以T
        "T",  //time
        //"U",  //auxiliary
        "VD",  //副动词直接作状语的动词。动词和副词的代码并在一起
        //"VG",  //动语素动词性语素。动词代码为v。在语素的代码g前面置以V
        "VN"  //名动词指具有名词功能的动词。动词和名词的代码并在一起
        "V",  //动词verb
        //"W",  //标点符号
        //"X",  //非语素字非语素字只是一个符号，字母x通常用于代表未知数、符号
        //"Y",  //语气词汉字“语”的声母
        //"Z",  //状态词:酷热空空闪闪甜甜取汉字'状'的声母的前一个字母
        "un", // 未知
        "" // Reserved one, don't Remove. It will be ignored when added
    };

    int size = sizeof(defaultPOS) / sizeof(std::string) - 1; // -1 is remove the last one

    for(int i=0; i<size; ++i)
        posList.push_back(defaultPOS[i]);
}

}
