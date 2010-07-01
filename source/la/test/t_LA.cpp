/**
 * @brief   Test code for LA library. 
 * @author  MyungHyun Lee (Kent)
 * @date    Aug 21, 2009
 */

#include "test_def.h"

#include <la/LA.h>
#include <la/UpdateDictThread.h>

using namespace la;
using namespace izenelib::util;
using namespace std;

enum ANALYSIS { 
#ifdef USE_WISEKMA
    KMA, 
#endif
#ifdef USE_IZENECMA
    CMA, 
#endif
    MULTI, // multi-language
    EMA,
    TOKEN, 
    NGRAM, 
    MATRIX,
    INIT };

void printTermList( const set<UString> & tlist )
{
    set<UString>::const_iterator it;
    for( it = tlist.begin(); it != tlist.end(); it ++ )
    {
        cout << "R: "; it->displayStringValue( UString ::UTF_8 ); cout << endl;
    }
}

int main( int argc, char * argv[] )
{
    // print the prompt
    ANALYSIS analysis = INIT;
    cout<<"Usage: t_LA [";
#ifdef USE_WISEKMA
    cout<<"kma(default)|";
    analysis = KMA;
#endif

#ifdef USE_IZENECMA
    if( analysis == INIT )
    {
        analysis = CMA;
        cout<<"cma(default)|";
    }
    else
    {
        cout<<"cma|";
    }
#endif

	cout << "multi|ema|token|ngram(2)|matrix] [encoding: euckr|utf8(default)] [case|nocase(default)]" << endl << endl;

    // DEFAULT CONFIGURATIONS
    IO_ENCODING = UString::UTF_8;

    if( argc > 1 )
    {
        string str( argv[1] );
        downcase( str );

        if( str == "token" )
        {
            analysis = TOKEN;
        }
#ifdef USE_WISEKMA
        else if( str == "kma" )
        {
            analysis = KMA;
        }
#endif
#ifdef USE_IZENECMA
        else if( str == "cma" )
        {
    		analysis = CMA;
        }
#endif
        else if( str == "multi" )
        {
            analysis = MULTI;
        }
        else if( str == "ema" )
        {
            analysis = EMA;
        }
        else if( str == "ngram" )
        {
            analysis = NGRAM;
        }
        else if( str == "matrix" )
        {
            analysis = MATRIX;
        }
        else
        {
            cout << "wrong analysis type: " << argv[1] << endl;
            return -1;
        }
    }
    // READING COMMAND ARGS
    if( argc > 2 )
    {
        if( strcmp(argv[2], "euckr") == 0 )
        {
            IO_ENCODING = UString::EUC_KR;
        }
        else if( strcmp(argv[2], "utf8") == 0 )
        {
            IO_ENCODING = UString::UTF_8;
        }
        else
        {
            cout << "wrong encoding type: " << argv[2] << endl;
            return -1;
        }
    }

    bool caseSensitive = false;
    if( argc > 3)
    {
        if( strcmp(argv[3], "case") == 0)
            caseSensitive = true;
    }

    // SETTING UP VARIABLES

    LA la;

    TokenizeConfig config;
    config.addUnites("=");
    config.addAllows("-");
    config.addAllows("|");
    //config.addAllows(",，");

    la.setTokenizerConfig( config );


    boost::shared_ptr<Analyzer> analyzer;

    switch ( analysis )
    {
#ifdef USE_WISEKMA
    case KMA:
            typedef CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul> NKoreanAnalyzer;
        	cout << "ANALYSIS: [korean(KMA)]:"  << endl;
        	analyzer.reset( new NKoreanAnalyzer( getKmaKnowledgePath() ) );
            static_cast<NKoreanAnalyzer*>(analyzer.get())->setGenerateCompNoun( true );
            static_cast<NKoreanAnalyzer*>(analyzer.get())->setExtractChinese( true );
            static_cast<NKoreanAnalyzer*>(analyzer.get())->setRetFlag_search( Analyzer::ANALYZE_ALL_ );
            static_cast<NKoreanAnalyzer*>(analyzer.get())->setIndexSynonym( true );
            static_cast<NKoreanAnalyzer*>(analyzer.get())->setSearchSynonym( true );
            //(static_cast<KoreanAnalyzer*>(analyzer.get()))->setGenerateCompNoun(true);
            //(static_cast<KoreanAnalyzer*>(analyzer.get()))->setExtractChinese(true);
            (static_cast<NKoreanAnalyzer*>(analyzer.get()))->setSpecialChars("|");
            break;
#endif
        case MULTI:
        {
            cout << "ANALYSIS: [MULTI]"  << endl;
            analyzer.reset( new MultiLanguageAnalyzer() );
            //static_cast<MultiLanguageAnalyzer*>(analyzer.get())->setExtractEngStem( false );
            static_cast<MultiLanguageAnalyzer*>(analyzer.get())->setContainLower( false );
            //static_cast<MultiLanguageAnalyzer*>(analyzer.get())->setProcessMode(
            //    MultiLanguageAnalyzer::ENGLISH, MultiLanguageAnalyzer::STRING_PM );

#ifdef USE_WISEKMA
            typedef CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul> NKoreanAnalyzer;
            shared_ptr<Analyzer> krSPtr;
            krSPtr.reset( new NKoreanAnalyzer( getKmaKnowledgePath() ) );
            static_cast<MultiLanguageAnalyzer*>(analyzer.get())->setDefaultAnalyzer( krSPtr );
            //(static_cast<NKoreanAnalyzer*>(krSPtr.get()))->setSpecialChars(".");
        #ifdef USE_IZENECMA
                typedef CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence> NChineseAnalyzer;
                shared_ptr<Analyzer> cnSPtr;
                cnSPtr.reset( new NChineseAnalyzer( CMA_KNOWLEDGE, false ) );
                (static_cast<NChineseAnalyzer*>(cnSPtr.get()))->setAnalysisType( 2 );
                cnSPtr.get()->setRetFlag_index( Analyzer::ANALYZE_SECOND_ );
                cnSPtr.get()->setRetFlag_search( Analyzer::ANALYZE_SECOND_ );
                (static_cast<NChineseAnalyzer*>(cnSPtr.get()))->setGenerateCompNoun( false );
                (static_cast<NChineseAnalyzer*>(cnSPtr.get()))->setExtractChinese( false );
                (static_cast<NChineseAnalyzer*>(cnSPtr.get()))->setLabelMode();
                static_cast<MultiLanguageAnalyzer*>(analyzer.get())->setAnalyzer(
                        MultiLanguageAnalyzer::CHINESE, cnSPtr );
                static_cast<MultiLanguageAnalyzer*>(analyzer.get())->setProcessMode(
                        MultiLanguageAnalyzer::CHINESE, MultiLanguageAnalyzer::MA_PM );

                shared_ptr<Analyzer> inSPtr = static_cast<MultiLanguageAnalyzer*>(analyzer.get())->getAnalyzer(
                        MultiLanguageAnalyzer::CHINESE );
    #else
                static_cast<MultiLanguageAnalyzer*>(analyzer.get())->setProcessMode(
                        MultiLanguageAnalyzer::CHINESE, MultiLanguageAnalyzer::CHARACTER_PM );
    #endif
#else
    #ifdef USE_IZENECMA
                typedef CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence> NChineseAnalyzer;
                shared_ptr<Analyzer> cnSPtr;
                cnSPtr.reset( new NChineseAnalyzer( CMA_KNOWLEDGE ) );
                cnSPtr.get()->setRetFlag_index( Analyzer::ANALYZE_SECOND_ );
                cnSPtr.get()->setRetFlag_search( Analyzer::ANALYZE_SECOND_ );
                (static_cast<NChineseAnalyzer*>(cnSPtr.get()))->setGenerateCompNoun( false );
                (static_cast<NChineseAnalyzer*>(cnSPtr.get()))->setExtractChinese( false );
                (static_cast<NChineseAnalyzer*>(cnSPtr.get()))->setLabelMode();
                static_cast<MultiLanguageAnalyzer*>(analyzer.get())->setDefaultAnalyzer( cnSPtr );
    #else
                cerr<<"[Error]At least assign one MA"<<endl;
                exit(1);
    #endif

#endif
            break;
        }

#ifdef USE_IZENECMA
        case CMA:
        {
        	typedef CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence> NChineseAnalyzer;
            cout << "ANALYSIS: [CMA]"  << endl;
            analyzer.reset( new NChineseAnalyzer( CMA_KNOWLEDGE ) );
            analyzer.get()->setRetFlag_index( Analyzer::ANALYZE_SECOND_ );
            analyzer.get()->setRetFlag_search( Analyzer::ANALYZE_SECOND_ );
            (static_cast<NChineseAnalyzer*>(analyzer.get()))->setGenerateCompNoun( false );
            (static_cast<NChineseAnalyzer*>(analyzer.get()))->setExtractChinese( false );
            (static_cast<NChineseAnalyzer*>(analyzer.get()))->setLabelMode();
            break;
        }
#endif
        case EMA:
        {
            analyzer.reset( new StemAnalyzer("english") );
            break;
        }
        case TOKEN:
            cout << "ANALYSIS: [token]"  << endl;
            break;
        case NGRAM:
            {
                unsigned int min = 0, max = 0;
                min = 2;
                max = 2;
                printf( "ANALYSIS: [ngram (min:%d max:%d)]\n", min, max );
                analyzer.reset( new NGramAnalyzer( min, max, 16 ) );
                static_cast<NGramAnalyzer*>(analyzer.get())->setApartFlag( NGramAnalyzer::NGRAM_APART_ALL_ );
            }
            break;
        case MATRIX:
            cout << "ANALYSIS: [matrix]"  << endl;
            analyzer.reset( new MatrixAnalyzer( true, true ) );
            break;

        default:
            cerr<<"[Error]At least assign one MA"<<endl;
            exit(1);
            break;
    }

    if( analyzer.get() != NULL )
    {
        //analyzer->setRetFlag_index( Analyzer::ANALYZE_SECOND_ );
        //analyzer->setRetFlag_search( Analyzer::ANALYZE_ALL_ );
        analyzer.get()->setCaseSensitive( caseSensitive );
    }
    la.setAnalyzer( analyzer );

    // start the update thread
    UpdateDictThread::staticUDT.setCheckInterval(2);
    cout<<"Start Global Update thread: "<<UpdateDictThread::staticUDT.start()<<endl;

    TermList::iterator it; 
    TermList termList;      // list of all the terms
    UString exp;
    TermList special;
    TermList prim;
    TermList second;
    set<UString> restrictList;
    


    while( true )
    {
        izenelib::util::UString query = showMenu();

        /*
        //cout << " Tokenizing only" << endl;
        cout << "@@-------------- process( query, termlist ) ---------------" << endl;
        la.process( query, termList, true );
        printTermList( termList );
        cout << endl;


        cout << "@@-------------- process( query, expquery, termlist ) ---------------" << endl;

        //la.process( query, exp, termList );
        la.process( query, exp, restrictList );
        cout << "EXP QUERY: "; exp.displayStringValue( IO_ENCODING ); cout << endl;
        //printTermList( termList );
        printTermList( restrictList );
        cout << endl;
        */

        cout << "-------------- process_index( query, termlist ) ---------------" << endl;
        la.process_index( query, termList );
        printTermList( termList );
        cout << endl;

        cout << "-------------- process_search( query, termlist ) ---------------" << endl;
        la.process_search( query, termList );
        printTermList( termList );
        cout << "Query string: "; toExpandedString( termList ).displayStringValue( UString::UTF_8 ); cout << endl;
        cout << endl;

        cout << "-------------- process( query, special, prim, second ) ---------------" << endl;
        //(static_cast<KoreanAnalyzer*>(analyzer.get()))->setIndexMode();
        la.process_index( query, special, prim, second );
        cout << "1. SPECIAL TERMS " << endl;
        printTermList( special );
        cout << "2. PRIM TERMS " << endl;
        printTermList( prim );
        cout << "3. SECOND TERMS " << endl;
        printTermList( second );
        cout << endl;


        cout << "------------- Combine SPECIAL TERMS and SECOND TERMS -----------------" << endl;
        appendPlaceHolder( second, special );
        printTermList( second );
/*
        shared_ptr<LA::PunctsType> puncts;
        puncts.reset( new LA::PunctsType );
        (*puncts)[(UCS2Char)','] = true;

        cout << "-------------- process_MIA_index( query, termlist ) ---------------" << endl;
        la.process_MIA_index( query, termList, puncts );
        printTermList( termList );
        cout << endl;

        cout << "-------------- process_MIA_search( query, termlist ) ---------------" << endl;
        la.process_MIA_search( query, termList, puncts );
        printTermList( termList );
        cout << "Query string: "; toExpandedString( termList ).displayStringValue( UString::UTF_8 ); cout << endl;
        cout << endl;
*/
    }





	return 1;
}
