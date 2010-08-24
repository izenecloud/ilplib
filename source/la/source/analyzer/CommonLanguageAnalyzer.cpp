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
    bExtractSynonym_(false)
{
    pStemmer_ = new stem::Stemmer();
    pStemmer_->init(stem::STEM_LANG_ENGLISH);

    lowercase_string_buffer_ = new char[string_buffer_size_];
    lowercase_ustring_buffer_ = new UString::CharT[ustring_buffer_size_];
    synonym_ustring_buffer_ = new UString::CharT[ustring_buffer_size_];
    stemming_ustring_buffer_ = new UString::CharT[ustring_buffer_size_];
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
    bExtractSynonym_(false)
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

    lowercase_string_buffer_ = new char[string_buffer_size_];
    lowercase_ustring_buffer_ = new UString::CharT[ustring_buffer_size_];
    synonym_ustring_buffer_ = new UString::CharT[ustring_buffer_size_];
    stemming_ustring_buffer_ = new UString::CharT[ustring_buffer_size_];
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

    while( nextToken() )
    {
        if( len() == 0 )
            continue;

//            {
//            string foo;
//            token().convertString(foo, UString::UTF_8);
//            cout << "(" << foo << ") --<> " << needIndex() << "  " << dec << endl;
//            }
        if( needIndex() )
        {
            if(isSpecialChar())
            {
                func( data, PLACE_HOLDER.c_str(), PLACE_HOLDER.length(), offset());
                continue;
            }

            const char* synonymInput = NULL;

            // foreign language, e.g. English
            if( isFL() )
            {
                UString::CharT* lowercaseTermUstr = lowercase_ustring_buffer_;
                bool lowercaseIsDifferent = UString::toLowerString(token(), len(),
                                            lowercase_ustring_buffer_, ustring_buffer_size_);

                char* lowercaseTerm = lowercase_string_buffer_;
                UString::convertString(UString::UTF_8, lowercaseTermUstr, len(), lowercase_string_buffer_, string_buffer_size_);

                if(bCaseSensitive_)
                {
                    func( data,  token(), len(), offset() );
                    if(bContainLower_ & lowercaseIsDifferent)
                    {
                        func( data, lowercaseTermUstr, len(), offset());
                    }
                }
                else
                {
                    func( data, lowercaseTermUstr, len(), offset());
                }

                if(bExtractEngStem_)
                {
                    /// TODO: write a UCS2 based stemmer
                    string stem_term;
                    pStemmer_->stem( lowercaseTerm, stem_term );
                    if( strcmp(stem_term.c_str(), lowercaseTerm) != 0 )
                    {
                        UString::CharT* stemmingTermUstr = stemming_ustring_buffer_;
                        size_t stemmingTermUstrSize = UString::toUcs2(UString::UTF_8,
                                stem_term.c_str(), stem_term.size(), stemming_ustring_buffer_, ustring_buffer_size_);
                        func( data, stemmingTermUstr, stemmingTermUstrSize, offset());
                    }
                }

                if(bExtractSynonym_)
                {
                    synonymInput = lowercaseTerm;
                }
            }
            else
            {
                func( data, token(), len(), offset());
                synonymInput = nativeToken();
            }

            if(bExtractSynonym_)
            {
                pSynonymContainer_ = uscSPtr_->getSynonymContainer();
                pSynonymContainer_->searchNgetSynonym( synonymInput, pSynonymResult_ );
                char * synonymResult = pSynonymResult_->getHeadWord(0);
                if( synonymResult )
                {
//                    cout << to_utf8(synonymInput,encode_) << "--<>" <<
//                         to_utf8(synonymResult, encode_) << "," << endl;
                    size_t synonymResultLen = strlen(synonymResult);
                    if(synonymResultLen <= ustring_buffer_size_)
                    {
                        UString::CharT * synonymResultUstr = synonym_ustring_buffer_;
                        size_t synonymResultUstrLen = UString::toUcs2(synonymEncode_,
                                synonymResult, synonymResultLen, synonym_ustring_buffer_, ustring_buffer_size_);
                        func( data, synonymResultUstr, synonymResultUstrLen, offset());
                    }
                }
            }
        }
    }
    return offset();
}

}
