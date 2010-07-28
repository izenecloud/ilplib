/** 
 * @file performance-test.cpp
 * @brief A test code for LAManager class.\n
 * @author MyungHyun (Kent)Lee
 * @date 2009-08-25
 *
 * @details
 *
 * Logs
 *
 * =====================  TEST SCHEME  =========================\n
 */


#include "test_def.h"

#include <la/LA.h>

#include <util/profiler/ProfilerGroup.h>

#include <boost/timer.hpp>

#include <list>
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;
using namespace la;
using namespace izenelib::util;

const unsigned int PRINT_ROUND = 10000;


int main( int argc, char * argv[] )
{
    if( argc < 3 )
    {

        cout<<"Usage: performance-test <file> <";
#ifdef USE_WISEKMA
        cout<<"all|";
#endif
#ifdef USE_IZENECMA
    	cout << "cma|";
#endif
    	cout << "tok|ngram> [file encoding utf8(default)|euckr] [check empty?]" << endl;

        return 0;
    }

    if( argc == 4 )
    {
        if( strcmp( argv[3], "utf8" ) == 0 )
        {
            FILE_ENCODING = izenelib::util::UString::UTF_8;
            cout << "File encoding type is utf8" << endl;
        }
        else if( strcmp( argv[3], "euckr" ) == 0 )
        {
            FILE_ENCODING = izenelib::util::UString::CP949;
            cout << "File encoding type is euckr(cp949)" << endl;
        }
        else
        {
            cout << "wrong encoding type:" << argv[3] << endl;
            return -1;
        }
    }
    else
    {
        cout << "File encoding type is utf8" << endl;
    }

    // la_allkor, la_nounkor, la_tok

    LA la;

    shared_ptr<Analyzer> analyzer;
    if( strcmp(argv[2], "ngram") == 0 )
    {
        analyzer.reset( new NGramAnalyzer(2, 2, 16) );
    }
#ifdef USE_WISEKMA
    else if( strcmp(argv[2], "all") == 0 )
    {
        typedef CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul> NKoreanAnalyzer;
        analyzer.reset( new NKoreanAnalyzer( getKmaKnowledgePath() ) );
        //analyzer.reset( new KoreanAnalyzer(getKmaKnowledgePath()) );
    }
#endif
    else if( strcmp(argv[2], "tok") == 0 )
    {
    }
#ifdef USE_IZENECMA
    else if( strcmp(argv[2], "cma") == 0 )
    {
    	typedef CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence> NChineseAnalyzer;
    	analyzer.reset( new NChineseAnalyzer( CMA_KNOWLEDGE ) );
    }
#endif
    else
    {
        cout << "wrong analysis name: " << argv[2] << endl;
        return -1;
    }

    la.setAnalyzer( analyzer );


    { // Setting tokenzier options
        /*

        TokenizeConfig config;

        config.addUnites("=");
        config.addAllows("-");
        la.setTokenizer( config );
        */
    }



    bool bCheckEmpty = false;
    if( argc > 5 )
    {
        if( parseTruth(argv[4]) == 1 )
            bCheckEmpty = true;
    }

    UString         query;
    TermList        termList;
    TermList        specialList;
    TermList        secondaryList;
    UString         expQueryString;

    set<UString> restrictList;



    //ifstream fin( "10K_E-parsed" );
    ifstream fin( argv[1] );
    stringstream ss;
    string buf;
    unsigned int i=0; 


    cout << "loading documents..." << endl;
    while( !fin.eof() )
    {
        getline( fin, buf );
        ss << buf << endl;
        i++;
    }
    fin.close();
    cout << "loaded " << i << " documents." << endl;

CREATE_PROFILER ( pro1, "LA", "Analyzing text : process(UString, TermList)");
CREATE_PROFILER ( pro2, "LA", "Analyzing text : process(UString, UString, TermList)");
CREATE_PROFILER ( pro3, "LA", "Analyzing text : process(UString, TermList, TermList, TermList )");

    int special_total = 0;
    int prim_total = 0;
    int second_total = 0;


    // #1 ------------- bool process( const UString &, TermList & ); -------------
    cout << "1. bool process( const UString &, TermList & );" << endl;

    clock_t etime = clock();
    while( !ss.eof() )
    {
        string line;
        getline( ss, line );
        UString ustr;
        ustr.assign( line, FILE_ENCODING );
        TermList llist;
        la.process_index( ustr, llist );
    }

    double dif = (double)(clock() - etime) / CLOCKS_PER_SEC;
    cout << "stream analysis time: " << dif << endl;
 /*
    i=0;
    {
        while( !ss.eof() )
        {
            if( i%PRINT_ROUND == 0)
            {
                //cout << ".";
                cout << i << endl;
            }
            getline( ss, buf );
            query.assign( buf, FILE_ENCODING );

START_PROFILER( pro1 );
            termList.clear();
            //la.process( query, termList, true );
            la.process_index( query, termList );
STOP_PROFILER( pro1 );
            prim_total += termList.size();
            if( bCheckEmpty )
            {
                TermList::iterator it = termList.begin();
                for( ; it != termList.end(); it++ ) 
                {
                    if( it->text_.length() == 0 )
                    {
                        cout << "TEXT IS EMPTY" << endl;
                        printTermList( termList );
                    }
                }
            }
            i++;
        }
    }
    */

    cout << "number of terms: " << prim_total << endl << endl;

    ss.clear();
    ss.seekg( 0, ios::beg );

    /*
    // #2 ------------- bool process( const UString &, UString &, TermList & ); -------------
    cout << "2. bool process( const UString &, UString &, TermList & );" << endl << endl;
    i=0; 
    prim_total = 0;
    termList.clear();
    {
        while( !ss.eof() )
        {
            if( i%PRINT_ROUND == 0)
            {
                //cout << ".";
                cout << i << endl;
            }
            getline( ss, buf );
            query.assign( buf, FILE_ENCODING );

START_PROFILER( pro2 );
            //la.process( query, expQueryString, termList );
            //la.process( query, expQueryString, restrictList );
            la.process_search( query, termList );
STOP_PROFILER( pro2 );
            prim_total += termList.size();
            //prim_total += restrictList.size();
            i++;
        }
    }
    cout << "number of restricted terms: " << prim_total << endl;

    ss.clear();
    ss.seekg( 0, ios::beg );
    */

    /*
    // #3 ------------- bool process( const UString &, TermList &, TermList &, TermList & ); -------------
    cout << "3. bool process( const UString &, TermList &, TermList &, TermList & );" << endl << endl;
    i=0; 
    prim_total = 0;
    termList.clear();
    {
        while( !ss.eof() )
        {
            if( i%PRINT_ROUND == 0)
            {
                //cout << ".";
                cout << i << endl;
            }
            getline( ss, buf );
            query.assign( buf, FILE_ENCODING );

START_PROFILER( pro3 );
            la.process( query, specialList, termList, secondaryList );
STOP_PROFILER( pro3 );
            special_total += specialList.size();
            prim_total += termList.size();
            second_total += secondaryList.size();
            i++;
        }
    }
    cout << "number of special terms: " << special_total << endl;
    cout << "number of prim terms: " << prim_total << endl;
    cout << "number of second terms: " << second_total << endl;
    */

    cout << "Time taken for: " << argv[2] << ":" << argv[1] << endl;

    REPORT_PROFILE_TO_SCREEN();
    return 1;
}







