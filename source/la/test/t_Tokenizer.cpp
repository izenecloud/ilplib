/*
 * t_Tokenizer.cpp
 *
 *  Created on: 2009-6-11
 *      Author: zhjay
 */
#include "test_def.h"

#include <Tokenizer.h>
using namespace la;
using namespace wiselib;
using namespace std;

void performTokenize( UString query, Tokenizer& tokenizer )
{
    TermList termList;
    TermList specialList;
    TermList::iterator it;

    cout << "-------------- PRIM TERMS ---------------" << endl;
    tokenizer.tokenize( query, termList );
    printTermList( termList );
    cout << endl;


    tokenizer.tokenize( query, specialList, termList );
    cout << "-------------- SPECIAL TERMS ---------------" << endl;
    printTermList( specialList );
    cout << "-------------- PRIM TERMS ---------------" << endl;
    printTermList( termList );
    cout << endl;

}


int main( int argc, char * argv[] )
{

    IO_ENCODING = UString::UTF_8;
    string specialChars;
    if( argc < 2 )
    {
        cout << "Usage: t_Tokenizer [special_chars]" << endl;
    }
    else
    {
        specialChars = argv[1];
        cout<<"Special Chars is \""<<specialChars<<"\""<<endl;
    }
    
    Tokenizer tokenizerAllow;
    Tokenizer tokenizerDivide;
    Tokenizer tokenizerUnite;

    if( specialChars.empty() == false )
    {
        TokenizeConfig configAllow;
        configAllow.addAllows( specialChars );
        tokenizerAllow.setConfig( configAllow );

        TokenizeConfig configDivide;
        configDivide.addDivides( specialChars );
        tokenizerDivide.setConfig( configDivide );

        TokenizeConfig configUnite;
        configUnite.addUnites( specialChars );
        tokenizerUnite.setConfig( configUnite );

    }


    while( true )
    {
        if( specialChars.empty() == true )
        {
            wiselib::UString query = showMenu();
            performTokenize( query, tokenizerAllow );
        }
        else
        {
            wiselib::UString query = showMenu();

            cout<<"############### When set \""<<specialChars<<"\" as allow: "<<endl;
            performTokenize( query, tokenizerAllow );

            cout<<"############### When set \""<<specialChars<<"\" as Divide: "<<endl;
            performTokenize( query, tokenizerDivide );

            cout<<"############### When set \""<<specialChars<<"\" as Unite: "<<endl;
            performTokenize( query, tokenizerUnite );
        }
    }

	return 1;
}
