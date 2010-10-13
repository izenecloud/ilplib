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
    cout << "Usage: ./t_LA [tok | char | cn | en | kr | multi | ngram | matrix | token(token analyzer)]" << endl;
}

boost::shared_ptr<Analyzer> getCMAAnalyzer()
{
    ChineseAnalyzer* cana = new ChineseAnalyzer( getCmaKnowledgePath() );
    cana->setLabelMode();
    cana->setAnalysisType(ChineseAnalyzer::minimum_match);
    boost::shared_ptr<Analyzer> cbana( cana );
    return cbana;
}

boost::shared_ptr<Analyzer> getKMAAnalyzer()
{
    KoreanAnalyzer* kana = new KoreanAnalyzer( getKmaKnowledgePath() );
    kana->setLabelMode();
    boost::shared_ptr<Analyzer> kbana( kana );
    return kbana;
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

    if( type == "char" )
    {
        analyzer.reset( new CharAnalyzer );
    }
    else if( type == "cn" )
    {
        ChineseAnalyzer* ana = new ChineseAnalyzer( getCmaKnowledgePath() );
        ana->setLabelMode();
        ana->setAnalysisType(ChineseAnalyzer::minimum_match);
        analyzer.reset( ana );
    }
    else if( type == "en" )
    {
        analyzer.reset( new EnglishAnalyzer );
    }
    else if( type == "kr" )
    {
        KoreanAnalyzer* ana = new KoreanAnalyzer( getKmaKnowledgePath() );
        ana->setLabelMode();
        analyzer.reset( ana );
    }
    else if( type == "multi" )
    {
        ChineseAnalyzer* cana = new ChineseAnalyzer( getCmaKnowledgePath() );
        cana->setLabelMode();
        cana->setAnalysisType(ChineseAnalyzer::minimum_match);
        boost::shared_ptr<Analyzer> cbana( cana );

        KoreanAnalyzer* kana = new KoreanAnalyzer( getKmaKnowledgePath() );
        kana->setLabelMode();
        boost::shared_ptr<Analyzer> kbana( kana );

        EnglishAnalyzer* eana = new EnglishAnalyzer;
        boost::shared_ptr<Analyzer> ebana( eana );

        MultiLanguageAnalyzer* mana = new MultiLanguageAnalyzer;
        mana->setDefaultAnalyzer( kbana );
        mana->setAnalyzer( MultiLanguageAnalyzer::CHINESE, cbana );
        mana->setAnalyzer( MultiLanguageAnalyzer::ENGLISH, ebana );

        analyzer.reset( mana );
    }
    else if( type == "ngram" )
    {
        cout << "NGramAnalyzer Addition Params: min max" << endl;
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
        cout << "[NGramAnalyzer] min = " << min << ", max = " << max << endl;
        analyzer.reset( new NGramAnalyzer( 2, 3, 1024000) );
    }
    else if( type == "matrix" )
    {
        analyzer.reset( new MatrixAnalyzer( true, true) );
    }
    else if( type == "token" )
    {
        analyzer.reset( new TokenAnalyzer );
    }
    else
    {
        cerr << "Unknown analyzer type: " << type << endl;
        exit(1);
    }

    la.setAnalyzer( analyzer );
    string input;
    do {
        cout << "Input ('x' to exit): ";
        getline( cin, input );
        if( input == "x" || input == "X" )
            break;

        UString ustr( input, IO_ENCODING );

        {
            TermList termList;
            cout << "-------------- process( query, termList ) ---------------" << endl;
            la.process( ustr, termList );
            printTermList( termList );
        }

    } while( true );

    return 0;
}
