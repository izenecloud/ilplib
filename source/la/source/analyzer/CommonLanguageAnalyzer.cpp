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
#include <la/common/Term.h>
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
    bChinese_(false),
    bRemoveStopwords_(false)
{
    pSynonymContainer_ = izenelib::am::VSynonymContainer::createObject();
    pSynonymContainer_->setSynonymDelimiter(",");
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

void CommonLanguageAnalyzer::analyzeSynonym(TermList& outList, size_t n)
{
    static UString SPACE(" ", izenelib::util::UString::UTF_8);
    TermList syOutList;

    size_t wordCount = outList.size();
    for(size_t i = 0; i < wordCount; i++)
    {
//        cout << "[off]" <<outList[i].wordOffset_<<" [level]"<<outList[i].getLevel() <<" [andor]" <<(unsigned int)(outList[i].getAndOrBit())
//             << "  "<< outList[i].textString()<<endl;

        // find synonym for word(s)
        for (size_t len = 1; (len <= n) && (i+len <= wordCount) ; len++)
        {
            // with space
            bool ret = false;
            UString combine;
            if (len > 1)
            {
                for (size_t j = 0; j < len-1; j++)
                {
                    combine.append(outList[i+j].text_);
                    combine.append(SPACE);
                }
                combine.append(outList[i+len-1].text_);
                ret = getSynonym(combine, outList[i].wordOffset_, Term::OR, outList[i].getLevel(), syOutList);
            }

            // without space
            if (!ret)
            {
                combine.clear();
                for (size_t j = 0; j < len; j++)
                    combine.append(outList[i+j].text_);
               ret = getSynonym(combine, outList[i].wordOffset_, Term::OR, outList[i].getLevel(), syOutList);
            }

            // adjust
            if (ret)
            {
                outList[i].setStats(outList[i].getAndOrBit(), outList[i].getLevel()+1);
                for (size_t j = 1; j < len; j++)
                {
                    outList[i+j].wordOffset_ = outList[i].wordOffset_;
                    outList[i+j].setStats(outList[i].getAndOrBit(), outList[i].getLevel());
                }

                syOutList.back().setStats(Term::OR, syOutList.back().getLevel());
                break;
            }
        }

        syOutList.push_back(outList[i]);
    }

    outList.swap(syOutList);
}

bool CommonLanguageAnalyzer::getSynonym(
        const UString& combine,
        int offset,
        const unsigned char andOrBit,
        const unsigned int level,
        TermList& syOutList)
{
    bool ret = false;
    //cout << "combined: "; combine.displayStringValue( izenelib::util::UString::UTF_8 ); cout << endl;

    char* combineStr = lowercase_string_buffer_;
    UString::convertString(UString::UTF_8, combine.c_str(), combine.length(), lowercase_string_buffer_, term_string_buffer_limit_);

    //cout << "combined string: " << string(combineStr) << endl;

    UString::CharT * synonymResultUstr = NULL;
    size_t synonymResultUstrLen = 0;

    pSynonymContainer_ = uscSPtr_->getSynonymContainer();
    pSynonymContainer_->searchNgetSynonym( combineStr, pSynonymResult_ );

    unsigned int subLevel = 1;
    for (int i =0; i<pSynonymResult_->getSynonymCount(0); i++)
    {
        char * synonymResult = pSynonymResult_->getWord(0, i);
        if( synonymResult )
        {
            if (strcmp(combineStr, synonymResult) == 0)
            {
                //cout << "synonym self: "<<string(synonymResult) <<endl;
                continue;
            }
            cout << "synonym : "<<string(synonymResult) <<endl;
            ret = true;

            size_t synonymResultLen = strlen(synonymResult);
            if(synonymResultLen <= term_ustring_buffer_limit_)
            {
                synonymResultUstr = synonym_ustring_buffer_;
                synonymResultUstrLen = UString::toUcs2(synonymEncode_,
                        synonymResult, synonymResultLen, synonym_ustring_buffer_, term_ustring_buffer_limit_);
            }

            // word segmentment
            UString term(synonymResultUstr, synonymResultUstrLen);
            TermList termList;
            if (innerAnalyzer_.get())
            {
                innerAnalyzer_->analyze(term, termList);
                if (termList.size() <= 1)
                {
                    syOutList.add(synonymResultUstr, synonymResultUstrLen, offset, NULL, andOrBit, level);
                }
                else
                {
                    for(TermList::iterator iter = termList.begin(); iter != termList.end(); ++iter)
                    {
                        syOutList.add(iter->text_.c_str(), iter->text_.length(), offset, NULL, Term::AND, level+1+subLevel);
                    }
                    subLevel++;
                }
            }
            else
            {
                syOutList.add(synonymResultUstr, synonymResultUstrLen, offset, NULL, andOrBit, level);
            }
        }
    }

    return ret;
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

        if( bRemoveStopwords_ && isStopword() )
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
                if(bExtractSynonym_)
                {
                    UString::CharT * synonymResultUstr = NULL;
                    size_t synonymResultUstrLen = 0;

                    pSynonymContainer_ = uscSPtr_->getSynonymContainer();
                    pSynonymContainer_->searchNgetSynonym( nativeToken(), pSynonymResult_ );

                    bool hasSynonym = false;
                    for (int i =0; i<pSynonymResult_->getSynonymCount(0); i++)
                    {
                        char * synonymResult = pSynonymResult_->getWord(0, i);
                        if( synonymResult )
                        {
                            if (strcmp(nativeToken(), synonymResult) == 0)
                            {
                                cout << "synonym self: "<<string(synonymResult) <<endl;
                                continue;
                            }
                            cout << "synonym : "<<string(synonymResult) <<endl;

                            size_t synonymResultLen = strlen(synonymResult);
                            if(synonymResultLen <= term_ustring_buffer_limit_)
                            {
                                synonymResultUstr = synonym_ustring_buffer_;
                                synonymResultUstrLen = UString::toUcs2(synonymEncode_,
                                        synonymResult, synonymResultLen, synonym_ustring_buffer_, term_ustring_buffer_limit_);
                            }

                            hasSynonym = true;
                            func( data, synonymResultUstr, synonymResultUstrLen, offset(), NULL, Term::OR, level()+1, false);
                        }
                    }

                    if (hasSynonym)
                    {
                        func( data, token(), len(), offset(), pos(), Term::OR, level()+1, false);
                    }
                    else
                    {
                        func( data, token(), len(), offset(), pos(), topAndOrBit, level(), false);
                    }
                }
                else
                {
                    func( data, token(), len(), offset(), pos(), topAndOrBit, level(), false);
                }
            }
        }
    }
    return offset();
}

}
