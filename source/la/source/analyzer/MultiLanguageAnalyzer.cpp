/**
 * @brief Analyzer for multi-languages
 * @file MultiLanguageAnalyzer.h
 *
 * @date Jan 4, 2010
 * @author vernkin
 */

#include "la/analyzer/MultiLanguageAnalyzer.h"

#include <iostream>
using namespace std;
using izenelib::util::UString;
using izenelib::util::UCS2Char;
using namespace izenelib::ir::idmanager;

namespace la
{

string LANG_CHAR_NAME[ MultiLanguageAnalyzer::OTHER ] =
{
    "CN_CHAR",
    "JP_CHAR",
    "KR_CHAR",
    "EN_CHAR",
};

string LANG_STRING_NAME[ MultiLanguageAnalyzer::OTHER ] =
{
    "FL",
    "FL",
    "FL",
    "FL",
};

MultiLanguageAnalyzer::MultiLanguageAnalyzer()
    : bExtractEngStem_( true ),
    pStemmer_( NULL )
{
    for( int i =0; i < OTHER; ++i )
    {
        modes_[i] = NONE_PM;
    }

    pStemmer_ = new stem::Stemmer();
    pStemmer_->init(stem::STEM_LANG_ENGLISH);
}

MultiLanguageAnalyzer::~MultiLanguageAnalyzer()
{
    delete pStemmer_;
}
//
//void MultiLanguageAnalyzer::setCaseSensitive( bool flag = true )
//{
//    bCaseSensitive_ = flag;
//    if( defAnalyzer_.get() != NULL )
//        defAnalyzer_.get()->setCaseSensitive( flag );
//    for( int i = 0; i< OTHER; ++i )
//    {
//        if( analyzers_[i].get() != NULL )
//            analyzers_[i].get()->setCaseSensitive( flag );
//    }
//}
//
//void MultiLanguageAnalyzer::setContainLower( bool flag = true )
//{
//    bContainLower_ = flag;
//    if( defAnalyzer_.get() != NULL )
//        defAnalyzer_.get()->setContainLower( flag );
//    for( int i = 0; i< OTHER; ++i )
//    {
//        if( analyzers_[i].get() != NULL )
//            analyzers_[i].get()->setContainLower( flag );
//    }
//}

bool MultiLanguageAnalyzer::setAnalyzer( Language lang, shared_ptr<Analyzer>& analyzer )
{
    if( lang == OTHER )
        return false;
    analyzers_[ lang ] = analyzer;
    initAnalyzerSetting( analyzer );
    return analyzer.get() != 0;
}

shared_ptr<Analyzer> MultiLanguageAnalyzer::getAnalyzer( MultiLanguageAnalyzer::Language lang ) const
{
    if( lang == OTHER )
        return shared_ptr<Analyzer>();
    return analyzers_[ lang ];
}


void MultiLanguageAnalyzer::setDefaultAnalyzer( shared_ptr<Analyzer>& defAnalyzer )
{
    defAnalyzer_ = defAnalyzer;
    initAnalyzerSetting( defAnalyzer );
}

shared_ptr<Analyzer> MultiLanguageAnalyzer::getDefaultAnalyzer() const
{
    return defAnalyzer_;
}

bool MultiLanguageAnalyzer::setProcessMode( Language lang, ProcessMode mode )
{
    if( lang == OTHER || (mode == MA_PM && analyzers_[lang].get() == NULL) )
        return false;
    modes_[ lang ] = mode;
    return true;
}

MultiLanguageAnalyzer::Language MultiLanguageAnalyzer::getCharType( UCS2Char ucs2Char )
{
    if( modes_[ CHINESE ] != NONE_PM && UString::isThisChineseChar( ucs2Char ) )
        return CHINESE;

    if( modes_[ ENGLISH ] != NONE_PM && UString::isThisAlphaChar( ucs2Char ) )
        return ENGLISH;

    if( modes_[ KOREAN ] != NONE_PM && UString::isThisKoreanChar( ucs2Char ) )
        return KOREAN;

    if( modes_[ JAPANESE ] != NONE_PM && UString::isThisJapaneseChar( ucs2Char ) )
        return JAPANESE;

    return OTHER;
}
//
//void MultiLanguageAnalyzer::invokeMA( const UString& ustr, TermList & output, bool isIndex,
//                                      Language lang, unsigned int woffset, unsigned int &listOffset, bool isEnd)
//{
//#ifdef DEBUG_MLA
//    cout<<"Str ";
//    ustr.displayStringInfo( UString::UTF_8, cout );
//    cout<<" with lang "<<lang<<endl;
//#endif
//    TermList::iterator termItr;
//    // Use default analyzer
//    if( lang == OTHER )
//    {
//        TermList input;
//        termItr = input.insert( input.end(), newTerm_ );
//        termItr->text_ = ustr;
//        termItr->wordOffset_ = woffset + listOffset;
//        if( isIndex )
//            listOffset += defAnalyzer_->analyze_index( input, output );
//        else
//            listOffset += defAnalyzer_->analyze_search( input, output );
//        if( !isEnd )
//            ++listOffset;
//        return;
//    }
//
//    ProcessMode mode = modes_[ lang ];
//
//    switch( mode )
//    {
//    case STRING_PM:
//    {
//        // process English
//        if( lang == ENGLISH )
//        {
//            unsigned int offset = woffset + listOffset;
//            string& pos = LANG_STRING_NAME[ lang ];
//
//            // insert original form
//            termItr = output.insert( output.end(), newTerm_ );
//            termItr->text_ = ustr;
//            termItr->wordOffset_ = offset;
////            termItr->pos_ = pos;
//
//            izenelib::util::UString lowerUstr = ustr;
//            // if it is not case sensitive, it has been in lower case
//            if( bCaseSensitive_  )
//            {
//                lowerUstr.toLowerString();
//                // add lower form in case of indexing and not equals to original form
//                if( isIndex && bContainLower_  && lowerUstr != ustr )
//                {
//                    termItr = output.insert( output.end(), newTerm_ );
//                    termItr->text_ = lowerUstr;
//                    termItr->wordOffset_ = offset;
////                    termItr->pos_ = pos;
//                }
//            }
//
//            if( bExtractEngStem_ )
//            {
//                string strUstr;
//                lowerUstr.convertString( strUstr, izenelib::util::UString::UTF_8 );
//                string stemstr;
//                pStemmer_->stem( strUstr, stemstr );
//                if( strUstr != stemstr )
//                {
//                    termItr = output.insert( output.end(), newTerm_ );
//                    termItr->text_.assign( stemstr, izenelib::util::UString::UTF_8 );
//                    termItr->wordOffset_ = offset;
////                    termItr->pos_ = pos;
//                }
//            }
//        }
//        else
//        {
//            termItr = output.insert( output.end(), newTerm_ );
//            termItr->text_ = ustr;
//            termItr->wordOffset_ = woffset + listOffset;
////            termItr->pos_ = LANG_STRING_NAME[ lang ];
//        }
//
//        if( !isEnd )
//            ++listOffset;
//        return;
//    }
//    case CHARACTER_PM:
//    {
//        size_t size = ustr.length();
//        for( size_t i = 0; i < size; ++i )
//        {
//            termItr = output.insert( output.end(), newTerm_ );
//            termItr->text_ = ustr.substr( i, 1);
//            termItr->wordOffset_ = woffset + listOffset;
////            termItr->pos_ = LANG_CHAR_NAME[ lang ];
//            if( !isEnd || (i + 1) < size )
//                ++listOffset;
//        }
//        return;
//    }
//    case MA_PM:
//    {
//        TermList input;
//        termItr = input.insert( input.end(), newTerm_ );
//        termItr->text_ = ustr;
//        termItr->wordOffset_ = woffset + listOffset;
//        if( isIndex )
//        {
//            listOffset += analyzers_[lang]->analyze_index( input, output );
//        }
//        else
//        {
//            listOffset += analyzers_[lang]->analyze_search( input, output );
//        }
//        if( !isEnd )
//            ++listOffset;
//        return;
//    }
//    default:
//        return;// do noting
//    }
//
//
//}

//void MultiLanguageAnalyzer::performAnalyze( const TermList & input, TermList & output, bool isIndex)
//{
//#ifdef DEBUG_MLA
//    printMLA();
//#endif
//    unsigned int listOffset = 0;
//    for( TermList::const_iterator itr = input.begin(); itr != input.end(); ++itr )
//    {
//        const UString& ustr = itr->text_;
//        size_t size = ustr.length();
//        if( size == 0 )
//            continue;
//
//        Language preType = getCharType( ustr[0] );
//        size_t beginIdx = 0;
//        size_t endIdx = 1;
//        for( ; endIdx < size; ++endIdx )
//        {
//            Language type = getCharType( ustr[ endIdx ] );
//            if( preType == type )
//                continue;
//
//            invokeMA( ustr.substr( beginIdx, endIdx - beginIdx ), output, isIndex, preType,
//                      itr->wordOffset_, listOffset, false );
//            beginIdx = endIdx;
//            preType = type;
//        }
//        if( beginIdx < endIdx )
//            invokeMA( ustr.substr( beginIdx, endIdx - beginIdx ), output, isIndex, preType,
//                      itr->wordOffset_, listOffset, true );
//    }
//}
//
//int MultiLanguageAnalyzer::analyze_index( const TermList & input, TermList & output,
//        unsigned char retFlag )
//{
//    performAnalyze( input, output, true );
//    return 0;
//}
//
//int MultiLanguageAnalyzer::analyze_search( const TermList & input, TermList & output,
//        unsigned char retFlag )
//{
//    performAnalyze( input, output, false );
//    return 0;
//}
//
//    template<typename IDManagerType>
//    int MultiLanguageAnalyzer::analyze_( IDManagerType* idm, const Term & input, TermIdList & output, analyzermode flags )
//    {
//        Language lang = CHINESE;
//        ProcessMode mode = modes_[CHINESE];
//        switch(mode)
//        {
//        case CHARACTER_PM:
//        {
//            unsigned int listOffset = 0;
//
//            const UString& ustr = input.text_;
//            size_t size = ustr.length();
//            if( size == 0 )
//                return 0;
//
//
//            for( size_t i = 0; i < size; ++i )
//            {
//                output.push_back(TermId());
//                ustr.substr( i, 1);
//    //            idm->getTermIdByTermString( ustr.substr( i, 1), output.back().termid_);
//                output.back().wordOffset_ = input.wordOffset_ + listOffset;
//                if((i + 1) < size )
//                    ++listOffset;
//            }
//            break;
//        }
//        case MA_PM:
//        {
//            analyzers_[lang]->analyze( idm, input, output);
//            break;
//        }
//        }
//        return 0;
//    }

void MultiLanguageAnalyzer::printMLA()
{
    cout<<"MLA, default Analyzer: " << defAnalyzer_.get() << endl;
    for( int i =0; i < OTHER; ++i )
    {
        cout<<"    Lang ID: " << i << ", PM Mode: " << modes_[i] <<
            ", analyzer : "<< analyzers_[i].get() << endl;
    }
}

void MultiLanguageAnalyzer::initAnalyzerSetting( boost::shared_ptr<Analyzer>& analyzer )
{
    if( analyzer.get() == NULL )
        return;
//    analyzer.get()->setCaseSensitive( bCaseSensitive_ );
//    analyzer.get()->setContainLower( bContainLower_ );
}

}
