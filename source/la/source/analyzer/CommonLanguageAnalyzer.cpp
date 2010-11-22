/**
 * @file    CommonLanguageAnalyzer.cpp
 * @author  Kent, Vernkin
 * @date    Nov 23, 2009
 * @details
 *  Common Language Analyzer for Chinese/Japanese/Korean.
 */

/**
 * @brief Rewrite CLA using new interfaces.
 * @author Wei
 */

////#define SF1_TIME_CHECK
//#include <util/profiler/ProfilerGroup.h>
//
#include <la/analyzer/CommonLanguageAnalyzer.h>
#include <la/util/UStringUtil.h>
//#include <la/dict/UpdateDictThread.h>
//
//using namespace izenelib::util;
//using namespace izenelib::ir::idmanager;
//using namespace std;
//
namespace la
{

CommonLanguageAnalyzer::CommonLanguageAnalyzer()
    : Analyzer(),
    pSynonymContainer_( NULL ),
    pSynonymResult_( NULL ),
    pStemmer_( NULL ),
    bCaseSensitive_(false),
    bContainLower_(false),
    bExtractEngStem_(false),
    bExtractSynonym_(false),
    bChinese_(false)
{
    pStemmer_ = new stem::Stemmer();
    pStemmer_->init(stem::STEM_LANG_ENGLISH);

    lowercase_string_buffer_ = new char[term_string_buffer_limit_];
    lowercase_ustring_buffer_ = new UString::CharT[term_ustring_buffer_limit_];
    synonym_ustring_buffer_ = new UString::CharT[term_ustring_buffer_limit_];
    stemming_ustring_buffer_ = new UString::CharT[term_ustring_buffer_limit_];
}

CommonLanguageAnalyzer::CommonLanguageAnalyzer(
        const std::string & synonymDictPath,
        UString::EncodingType synonymEncode)
    : Analyzer(),
    pSynonymContainer_( NULL ),
    pSynonymResult_( NULL ),
    synonymEncode_( synonymEncode ),
    pStemmer_( NULL ),
    bCaseSensitive_(false),
    bContainLower_(false),
    bExtractEngStem_(false),
    bExtractSynonym_(false),
    bChinese_(false)
{
    pSynonymContainer_ = izenelib::am::VSynonymContainer::createObject();
    pSynonymContainer_->setSynonymDelimiter(" ");
    pSynonymContainer_->setWordDelimiter("_");
    if( pSynonymContainer_->loadSynonym( synonymDictPath.c_str() ) != 1 )
    {
        string msg = "Failed to load synonym dictionary from path: ";
        msg += synonymDictPath;
        throw std::logic_error( msg );
    }

    uscSPtr_.reset( new UpdatableSynonymContainer( pSynonymContainer_, synonymDictPath ) );
    UpdateDictThread::staticUDT.addRelatedDict( synonymDictPath.c_str(), uscSPtr_ );

    pSynonymResult_ = izenelib::am::VSynonym::createObject();

    pStemmer_ = new stem::Stemmer();
    pStemmer_->init(stem::STEM_LANG_ENGLISH);

    lowercase_string_buffer_ = new char[term_string_buffer_limit_];
    lowercase_ustring_buffer_ = new UString::CharT[term_ustring_buffer_limit_];
    synonym_ustring_buffer_ = new UString::CharT[term_ustring_buffer_limit_];
    stemming_ustring_buffer_ = new UString::CharT[term_ustring_buffer_limit_];
}

void CommonLanguageAnalyzer::setSynonymUpdateInterval(unsigned int seconds)
{
    UpdateDictThread::staticUDT.setCheckInterval(seconds);
    if( !UpdateDictThread::staticUDT.isStarted() )
        UpdateDictThread::staticUDT.start();
}

CommonLanguageAnalyzer::~CommonLanguageAnalyzer()
{
    delete pSynonymContainer_;
    delete pSynonymResult_;
    delete pStemmer_;

    delete lowercase_string_buffer_;
    delete lowercase_ustring_buffer_;
    delete synonym_ustring_buffer_;
    delete stemming_ustring_buffer_;
}

int CommonLanguageAnalyzer::analyze_impl( const Term& input, void* data, HookType func )
{
    parse(input.text_);

    unsigned char topAndOrBit = Term::AND;
    int lastWordOffset = -1;

    while( nextToken() )
    {
        if( len() == 0 )
            continue;

/*            {
            UString foo(token(), len()); string bar; foo.convertString(bar, UString::UTF_8);
            cout << "(" << bar << ") --<> " << isIndex() << "," << offset() << "," << isRaw() << "," << level() << endl;
            }*/

        if( bChinese_ == true )
        {
            int curWordOffset = offset();
            if( lastWordOffset == lastWordOffset )
                topAndOrBit = Term::OR;
            else
                topAndOrBit = Term::AND;
            lastWordOffset = curWordOffset;
        }

        if( isIndex() )
        {
            if(isSpecialChar())
            {
                func( data, token(), len(), offset(), Term::SpecialCharPOS, Term::AND, level(), true);
                continue;
            }
            if(isRaw())
            {
                func( data, token(), len(), offset(), pos(), Term::OR, level(), false);
                continue;
            }

            // foreign language, e.g. English
            if( isAlpha() )
            {
                UString::CharT* lowercaseTermUstr = lowercase_ustring_buffer_;
                bool lowercaseIsDifferent = UString::toLowerString(token(), len(),
                                            lowercase_ustring_buffer_, term_ustring_buffer_limit_);

                char* lowercaseTerm = lowercase_string_buffer_;
                UString::convertString(UString::UTF_8, lowercaseTermUstr, len(), lowercase_string_buffer_, term_string_buffer_limit_);

                UString::CharT* stemmingTermUstr = NULL;
                size_t stemmingTermUstrSize = 0;

                UString::CharT * synonymResultUstr = NULL;
                size_t synonymResultUstrLen = 0;

                if(bExtractEngStem_)
                {
                    /// TODO: write a UCS2 based stemmer
                    string stem_term;
                    pStemmer_->stem( lowercaseTerm, stem_term );
                    if( strcmp(stem_term.c_str(), lowercaseTerm) != 0 )
                    {
                        stemmingTermUstr = stemming_ustring_buffer_;
                        stemmingTermUstrSize = UString::toUcs2(UString::UTF_8,
                                stem_term.c_str(), stem_term.size(), stemming_ustring_buffer_, term_ustring_buffer_limit_);
                    }
                }

                if(bExtractSynonym_)
                {
                    pSynonymContainer_ = uscSPtr_->getSynonymContainer();
                    pSynonymContainer_->searchNgetSynonym( lowercaseTerm, pSynonymResult_ );
                    char * synonymResult = pSynonymResult_->getHeadWord(0);
                    if( synonymResult )
                    {
                        size_t synonymResultLen = strlen(synonymResult);
                        if(synonymResultLen <= term_ustring_buffer_limit_)
                        {
                            synonymResultUstr = synonym_ustring_buffer_;
                            synonymResultUstrLen = UString::toUcs2(synonymEncode_,
                                    synonymResult, synonymResultLen, synonym_ustring_buffer_, term_ustring_buffer_limit_);
                        }
                    }
                }

                if( stemmingTermUstr || synonymResultUstr || (bCaseSensitive_ && bContainLower_ && lowercaseIsDifferent) ) {
                    /// have more than one output
                    if(bCaseSensitive_) {
                        func( data,  token(), len(), offset(), Term::EnglishPOS, Term::OR, level()+1, false);
                    } else {
                        func( data, lowercaseTermUstr, len(), offset(), Term::EnglishPOS, Term::OR, level()+1, false);
                    }
                    if(stemmingTermUstr) {
                        func( data, stemmingTermUstr, stemmingTermUstrSize, offset(), Term::EnglishPOS, Term::OR, level()+1, false);
                    }
                    if(synonymResultUstr) {
                        func( data, synonymResultUstr, synonymResultUstrLen, offset(), NULL, Term::OR, level()+1, false);
                    }
                    if(bCaseSensitive_ && bContainLower_ && lowercaseIsDifferent)
                    {
                        func( data, lowercaseTermUstr, len(), offset(), Term::EnglishPOS, Term::OR, level()+1, false);
                    }
                } else {
                    /// have only one output
                    if(bCaseSensitive_) {
                        func( data,  token(), len(), offset(), Term::EnglishPOS, Term::AND, level(), false);
                    } else {
                        func( data, lowercaseTermUstr, len(), offset(), Term::EnglishPOS, Term::AND, level(), false);
                    }
                }
            }
            else
            {
                UString::CharT * synonymResultUstr = NULL;
                size_t synonymResultUstrLen = 0;

                if(bExtractSynonym_)
                {
                    pSynonymContainer_ = uscSPtr_->getSynonymContainer();
                    pSynonymContainer_->searchNgetSynonym( nativeToken(), pSynonymResult_ );
                    char * synonymResult = pSynonymResult_->getHeadWord(0);
                    if( synonymResult )
                    {
                        size_t synonymResultLen = strlen(synonymResult);
                        if(synonymResultLen <= term_ustring_buffer_limit_)
                        {
                            synonymResultUstr = synonym_ustring_buffer_;
                            synonymResultUstrLen = UString::toUcs2(synonymEncode_,
                                    synonymResult, synonymResultLen, synonym_ustring_buffer_, term_ustring_buffer_limit_);
                        }
                    }
                }

                if(synonymResultUstr) {
                    func( data, token(), len(), offset(), pos(), Term::OR, level()+1, false);
                    func( data, synonymResultUstr, synonymResultUstrLen, offset(), NULL, Term::OR, level()+1, false);
                } else {
                    func( data, token(), len(), offset(), pos(), topAndOrBit, level(), false);
                }
            }
        }
    }
    return offset();
}

}
