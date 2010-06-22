/**
 * @file    UpdatableDictThread.h
 * @author  Vernkin
 * @date    Feb 2, 2009
 * @details
 *  Test the dynamic dictionary
 */

#include <iostream>

#include <la/PlainDictionary.h>
#include <la/UpdateDictThread.h>

using namespace la;
using namespace wiselib;

int main( int argc, char * argv[] )
{
    UString::EncodingType enc = UString::UTF_8;
    if( argc < 2 )
    {
        cout<<"Usage: t_DynamicDict dictPath [euckr|utf8(default)]"<<endl;
        return 1;
    }

    const char* path = argv[1];
    if( argc >= 3 )
    {
        if( strcmp( argv[2], "utf8" ) == 0 )
            enc = UString::UTF_8;
        else if( strcmp( argv[2], "euckr" ) == 0 )
            enc = UString::EUC_KR;
        else
        {
            cerr<<"Error, Unknown Encoding " << argv[2] << endl;
            return 1;
        }
    }

    cout<<"Load the PlainDictionary from "<<path<<endl;
    UpdateDictThread& udt = UpdateDictThread::staticUDT;
    udt.setCheckInterval( 2 );
    shared_ptr<PlainDictionary> pdPtr = udt.createPlainDictionary( path, enc );
    cout<<"Start UpdateDictThread: "<<udt.start()<<endl;

    string input;
    UString ustr;
    do{
        cout<<"Whether exists word ( 'x' or 'X' to exit): ";
        cin>>input;
        if( input == "x" || input == "X" )
            break;
        ustr.assign( input, enc );
        cout<<"Exists "<<input<<" : "<< pdPtr->contain( ustr )<<endl;
    }while(true);

    return 0;
}
