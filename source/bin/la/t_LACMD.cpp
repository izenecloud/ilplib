/**
 * \file t_LA.cpp
 * \brief 
 * \date Oct 13, 2010
 * \author Vernkin Chen
 */
#include <ilplib.hpp>
#include <string>
#include <util/ustring/UString.h>

#include "la_bin_test_def.h"

using namespace std;
using namespace la;
using namespace izenelib::util;

void testTokenizer()
{
    Tokenizer tokenizer;
    string input;
    do {
        cout << "Input ('x' to exit): ";
        getline( cin, input );
        if( input == "x" || input == "X" )
            break;

        UString ustr( input, IO_ENCODING );
        {
            TermList specialTerms;
            TermList primTerms;
            cout << "-------------- tokenize( query, specialTerms, primTerms ) ---------------" << endl;
            tokenizer.tokenize(ustr, specialTerms, primTerms );
            cout << "1.  SPECIAL TERMS" << endl;
            printTermList( specialTerms );
            cout << "2.  Primary TERMS" << endl;
            printTermList( primTerms );
        }


        {
            TermList rawTerms;
            cout << "-------------- tokenizeWhite( query, rawTerms ) ---------------" << endl;
            tokenizer.tokenizeWhite(ustr, rawTerms );
            printTermList( rawTerms );
        }

        {
            TermList primTerms;
            cout << "-------------- tokenize( query, primTerms ) ---------------" << endl;
            tokenizer.tokenize(ustr, primTerms );
            printTermList( primTerms );
        }

    } while( true );
}

void printUsage()
{
    cout << "Usage: ./t_LA [tok | char | cn | en | kr | multi | ngram | matrix | token(token analyzer) | none(empty analyzer)]" << endl;
}

boost::shared_ptr<Analyzer> getCMAAnalyzer()
{
    ChineseAnalyzer* cana = new ChineseAnalyzer( getCmaKnowledgePath() );
    cana->setLabelMode();
    cana->setAnalysisType(ChineseAnalyzer::minimum_match);
    boost::shared_ptr<Analyzer> cbana( cana );
    return cbana;
}

int main( int argc, char** argv )
{
    if( argc < 2 )
    {
        printUsage();
        exit(1);
    }

    string type = argv[1];
    if( type == "tok" )
    {
        testTokenizer();
        exit(0);
    }

    LA la;
    boost::shared_ptr<Analyzer> analyzer;

    bool extractSynonym = false;

    if( type == "char" )
    {
        cout << "CharAnalyzer Addition Params: [all | part] [tolowcase]" << endl;
        CharAnalyzer* ana = new CharAnalyzer;
        for( int i = 2; i < argc; ++i )
        {
            string arg = argv[i];
            if( arg == "all" )
                ana->setSeparateAll(true);
            else if( arg == "part" )
                ana->setSeparateAll(false);
            else if( arg == "tolowcase" )
                ana->setCaseSensitive(false);
            else
                cerr << "Invalid Parameter: " << arg << endl;
        }

        analyzer.reset( ana );
    }
    else if( type == "cn" )
    {
        cout << "ChineseAnalyzer Addition Params: [max | min | mmm | min_u] [label | index | stop | synonym | inner]" << endl;
        ChineseAnalyzer* ana = new ChineseAnalyzer( getCmaKnowledgePath() );
        ana->setLabelMode();
        ana->setCaseSensitive(false);
        ana->setAnalysisType(ChineseAnalyzer::minimum_match);

        for( int i = 2; i < argc; ++i )
        {
            string arg = argv[i];
            if( arg == "max" )
                ana->setAnalysisType(ChineseAnalyzer::maximum_match);
            else if( arg == "min" )
                ana->setAnalysisType(ChineseAnalyzer::minimum_match);
            else if( arg == "min_u" )
                ana->setAnalysisType(ChineseAnalyzer::minimum_match_with_unigram);
            else if( arg == "mmm" )
                ana->setAnalysisType(ChineseAnalyzer::maximum_entropy);
            else if( arg == "label" )
                ana->setLabelMode();
            else if( arg == "index" )
                ana->setIndexMode();
            else if( arg == "stop")
                ana->setRemoveStopwords();
            else if( arg == "synonym")
            {
                extractSynonym = true;
                //ana->setExtractSynonym(true);
            }
            else if( arg == "inner")
            {
                ChineseAnalyzer* inner_ana = new ChineseAnalyzer( getCmaKnowledgePath() );
                inner_ana->setLabelMode();
                inner_ana->setCaseSensitive(false);
                inner_ana->setAnalysisType(ChineseAnalyzer::minimum_match_no_overlap);
                inner_ana->setExtractSynonym(false);
                boost::shared_ptr<Analyzer> analyzer(inner_ana);
                ana->setInnerAnalyzer(analyzer);
            }
            else
                cerr << "Invalid Parameter: " << arg << endl;
        }

        analyzer.reset( ana );
    }
    else if( type == "en" )
    {
        analyzer.reset( new EnglishAnalyzer );
    }
    else if( type == "multi" )
    {
        ChineseAnalyzer* cana = new ChineseAnalyzer( getCmaKnowledgePath() );
        cana->setLabelMode();
        cana->setAnalysisType(ChineseAnalyzer::minimum_match);
        boost::shared_ptr<Analyzer> cbana( cana );

        EnglishAnalyzer* eana = new EnglishAnalyzer;
        boost::shared_ptr<Analyzer> ebana( eana );

        MultiLanguageAnalyzer* mana = new MultiLanguageAnalyzer;
        mana->setDefaultAnalyzer( cbana );
        mana->setAnalyzer( MultiLanguageAnalyzer::CHINESE, cbana );
        mana->setAnalyzer( MultiLanguageAnalyzer::ENGLISH, ebana );

        ilplib::langid::Factory* langIdFactory = ilplib::langid::Factory::instance();
        MultiLanguageAnalyzer::langIdAnalyzer_ = langIdFactory->createAnalyzer();
        ilplib::langid::Knowledge* langIdKnowledge_ = langIdFactory->createKnowledge();
        langIdKnowledge_->loadEncodingModel("../../db/langid/model/encoding.bin");
        langIdKnowledge_->loadLanguageModel("../../db/langid/model/language.bin");
        MultiLanguageAnalyzer::langIdAnalyzer_->setKnowledge(langIdKnowledge_);

        analyzer.reset( mana );
    }
    else if( type == "ngram" )
    {
        cout << "NGramAnalyzer Addition Params: min max flag" << endl;
        int min = 2;
        int max = 3;
        if( argc > 2 )
            min = atoi( argv[2] );
        if( argc > 3 )
            max = atoi( argv[3] );
        if( min < 1 )
            min = 2;
        if( max < min )
            max = min + 1;
        unsigned int apartFlag = 0;
        if( argc > 4 )
            apartFlag = (unsigned int)atoi( argv[4] );

        cout << "[NGramAnalyzer] min = " << min << ", max = " << max << ", apartFlag = "
                << hex << apartFlag << endl;
        analyzer.reset( new NGramAnalyzer( 2, 3, 1024000, apartFlag ) );
    }
    else if( type == "matrix" )
    {
        analyzer.reset( new MatrixAnalyzer( true, true) );
    }
    else if( type == "token" )
    {
        analyzer.reset( new TokenAnalyzer );
    }
    else if( type == "none" )
    {
        // do nothing
    }
    else
    {
        cerr << "Unknown analyzer type: " << type << endl;
        exit(1);
    }

    bool toHalfWidthString = false;
    for( int i = 2; i < argc; ++i ) {
    	string arg = argv[i];
    	if (arg == "half-width") {
    		toHalfWidthString = true;
    	}
    }

    la.setAnalyzer( analyzer );
    string input;
    do {
        cout << "Input ('x' to exit): ";
        getline( cin, input );
        if( input == "x" || input == "X" )
            break;

        UString ustr( input, IO_ENCODING );

        if ( toHalfWidthString )
        {
			la::convertFull2HalfWidth(ustr);
			string str;
			ustr.convertString(str, UString::UTF_8);
			cout << "Input (full-with chars are converted to half-with): " << str << endl;
        }

        {
            TermList termList;
            cout << "-------------- process( query, termList ) ---------------" << endl;
            la.process( ustr, termList );
            printTermList( termList );

            cout << "-------------- toExpandedString ---------------" << endl;
            UString exp = toExpandedString(termList);
            exp.displayStringInfo(UString::UTF_8, cout);
            cout << endl;

            if (extractSynonym)
            {
                cout << "============== process with synonym =====================" << endl;
                termList.clear();
                la.processSynonym( ustr, termList);
                printTermList( termList );

                cout << "============== toExpandedString ================" << endl;
                UString exp = toExpandedString(termList);
                exp.displayStringInfo(UString::UTF_8, cout);
                cout << endl;
            }
        }

    } while( true );

    return 0;
}
