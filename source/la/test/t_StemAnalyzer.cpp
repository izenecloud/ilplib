/*
 * t_StemAnalyzer.cpp
 *
 *  Created on: 2009-6-14
 *      Author: zhjay
 */
#include "test_def.h"

#include <la/analyzer/StemAnalyzer.h>
#include <la/tokenizer/Tokenizer.h>
#include <la/stem/Stemmer.h>
#include <ctime>
using namespace la;
using namespace izenelib::util;
using namespace std;
using namespace la::stem;

int main(){

    cout << "RUNNING TEST ON StemAnalyzer. (only alphabets are allowed as input." << endl;

	Tokenizer tokenizer;

    Analyzer * analyzer[STEM_LANG_EOS-1];

    analyzer[0] = new StemAnalyzer(STEM_LANG_DANISH);
    analyzer[1] = new StemAnalyzer(STEM_LANG_DUTCH);
    analyzer[2] = new StemAnalyzer(STEM_LANG_ENGLISH);
    analyzer[3] = new StemAnalyzer(STEM_LANG_FINNISH);
    analyzer[4] = new StemAnalyzer(STEM_LANG_FRENCH);
    analyzer[5] = new StemAnalyzer(STEM_LANG_GERMAN);
    analyzer[6] = new StemAnalyzer(STEM_LANG_HUNGARIAN);
    analyzer[7] = new StemAnalyzer(STEM_LANG_ITALIAN);
    analyzer[8] = new StemAnalyzer(STEM_LANG_NORWEGIAN);
    analyzer[9] = new StemAnalyzer(STEM_LANG_PORTUGUESE);
    analyzer[10] = new StemAnalyzer(STEM_LANG_ROMANIAN);
    analyzer[11] = new StemAnalyzer(STEM_LANG_RUSSIAN);
    analyzer[12] = new StemAnalyzer(STEM_LANG_SPANISH);
    analyzer[13] = new StemAnalyzer(STEM_LANG_SWEDISH);
    analyzer[14] = new StemAnalyzer(STEM_LANG_TURKISH);

    UString ustr;
    TermList tlist;
    TermList outlist;

    while( true )
    {
        ustr = showMenu();

        tokenizer.tokenize( ustr, tlist );

        analyzer[0]->analyze_index( tlist, outlist );
        cout << "DANISH: " << endl;
        printTermList( outlist );
        analyzer[1]->analyze_index( tlist, outlist );
        cout << "DUTCH: " << endl;
        printTermList( outlist );
        analyzer[2]->analyze_index( tlist, outlist );
        cout << "ENGLISH: " << endl;
        printTermList( outlist );
        analyzer[3]->analyze_index( tlist, outlist );
        cout << "FINNISH: " << endl;
        printTermList( outlist );
        analyzer[4]->analyze_index( tlist, outlist );
        cout << "FRENCH: " << endl;
        printTermList( outlist );
        analyzer[5]->analyze_index( tlist, outlist );
        cout << "GERMAN: " << endl;
        printTermList( outlist );
        analyzer[6]->analyze_index( tlist, outlist );
        cout << "HUNGARIAN: " << endl;
        printTermList( outlist );
        analyzer[7]->analyze_index( tlist, outlist );
        cout << "ITALIAN: " << endl;
        printTermList( outlist );
        analyzer[8]->analyze_index( tlist, outlist );
        cout << "NORWEIGIAN: " << endl;
        printTermList( outlist );
        analyzer[9]->analyze_index( tlist, outlist );
        cout << "PORTUGUESE: " << endl;
        printTermList( outlist );
        analyzer[10]->analyze_index( tlist, outlist );
        cout << "ROMANIAN: " << endl;
        printTermList( outlist );
        analyzer[11]->analyze_index( tlist, outlist );
        cout << "RUSSIAN: " << endl;
        printTermList( outlist );
        analyzer[12]->analyze_index( tlist, outlist );
        cout << "SPANISH: " << endl;
        printTermList( outlist );
        analyzer[13]->analyze_index( tlist, outlist );
        cout << "SWEDISH: " << endl;
        printTermList( outlist );
        analyzer[14]->analyze_index( tlist, outlist );
        cout << "TURKISH: " << endl;
        printTermList( outlist );

    }

	return 1;
}
