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
#include <am/vsynonym/StrBasedVTrie.h>
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
    uscSPtr_ = static_pointer_cast<UpdatableSynonymContainer>(UpdateDictThread::staticUDT.addRelatedDict( synonymDictPath.c_str(), uscSPtr_ ));
    if ( !uscSPtr_.get() )
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
    }
    else
        pSynonymContainer_ = uscSPtr_->getSynonymContainer();

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
            unsigned int subLevel = 0;
            UString combine;
            if (len > 1)
            {
                for (size_t j = 0; j < len-1; j++)
                {
                    combine.append(outList[i+j].text_);
                    combine.append(SPACE);
                }
                combine.append(outList[i+len-1].text_);
                ret = getSynonym(combine, outList[i].wordOffset_, Term::OR, outList[i].getLevel(), syOutList, subLevel);
            }

            // without space
            if (!ret)
            {
                combine.clear();
                for (size_t j = 0; j < len; j++)
                    combine.append(outList[i+j].text_);
               ret = getSynonym(combine, outList[i].wordOffset_, Term::OR, outList[i].getLevel(), syOutList, subLevel);
            }

            // adjust
            if (ret)
            {
                outList[i].setStats(outList[i].getAndOrBit(), outList[i].getLevel()+subLevel);
                for (size_t j = 1; j < len; j++)
                {
                    outList[i+j].wordOffset_ = outList[i].wordOffset_;
                    outList[i+j].setStats(outList[i+j].getAndOrBit(), outList[i].getLevel());
                }
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
        TermList& syOutList,
        unsigned int& subLevel)
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
                    syOutList.add(synonymResultUstr, synonymResultUstrLen, offset, NULL, andOrBit, level+subLevel);
                    subLevel++;
                }
                else
                {
                    for(TermList::iterator iter = termList.begin(); iter != termList.end(); ++iter)
                    {
                        syOutList.add(iter->text_.c_str(), iter->text_.length(), offset, NULL, Term::AND, level+subLevel);
                    }
                    subLevel++;
                }
            }
            else
            {
                syOutList.add(synonymResultUstr, synonymResultUstrLen, offset, NULL, andOrBit, level+subLevel);
                subLevel++;
            }
        }
    }

    return ret;
}

void displayBuffer(char* p, size_t n)
{
    for (size_t i =0; i< n; i++)
    {
        cout << *(p+i);
    }
    cout << endl;
}

bool CommonLanguageAnalyzer::analyze_synonym_impl(const izenelib::util::UString& inputString, SynonymOutputType& synonymOutput)
{
//    cout << "[CommonLanguageAnalyzer::analyze_synonym_impl] ";
//    inputString.displayStringValue(izenelib::util::UString::UTF_8);
//    cout << endl;

    bool retFoundSynonym = false;

    const UString::CharT* pInput = inputString.c_str();
    size_t length = inputString.length();

    // ensure length
    size_t ustring_buffer_size_ = term_ustring_buffer_limit_;
    if (ustring_buffer_size_ < length+1)
    {
        ustring_buffer_size_ = length+1;
        delete lowercase_ustring_buffer_;
        lowercase_ustring_buffer_ = new UString::CharT[ustring_buffer_size_];
    }
    size_t string_buffer_size = term_string_buffer_limit_;
    if(string_buffer_size < length*4)
    {
        string_buffer_size = length*4;
        delete lowercase_string_buffer_;
        lowercase_string_buffer_ = new char[string_buffer_size];
    }

    // to low case
    UString::CharT* lowercaseTermUstr = lowercase_ustring_buffer_;
    bool lowercaseIsDifferent = UString::toLowerString(pInput, length,
                                lowercase_ustring_buffer_, term_ustring_buffer_limit_);
    if (lowercaseIsDifferent)
        pInput = lowercaseTermUstr;

    // convert input string to utf8 characters
    char* chars = lowercase_string_buffer_;
    vector<size_t> charOffs; charOffs.reserve(length);
    size_t preCharLen = -1;
    size_t curOff = 0;
    for (size_t i = 0; i < length; i++)
    {
        curOff += (preCharLen+1);
        charOffs[i] = curOff;
        preCharLen = UString::convertString(UString::UTF_8, pInput+i, 1, chars+curOff, 4);
        //cout << chars+curOff <<" "<<curOff<<" " << preCharLen <<endl;
    }

    // search synonym dict for input
    izenelib::am::StrBasedVTrie strTrie(pSynonymContainer_->getData());
    UString::CharT * synonymResultUstr = NULL;
    size_t synonymResultUstrLen = 0;

    size_t curIdx = 0;
    size_t startIdx = 0;
    size_t wordEndIdx;
    VTrieNode endNode;
    while (curIdx < length)
    {
        wordEndIdx = size_t(-1);

        char* pch = chars+charOffs[curIdx];
        strTrie.firstSearch( pch );
        if (strTrie.completeSearch == false || strTrie.node->moreLong == false)
        {
            curIdx ++;
            continue;
        }

        if (strTrie.exists())
        {
            wordEndIdx = curIdx;
            strTrie.getCurrentNode(endNode);
        }
        //else // uncomment if minimum match
        {
            for (size_t j = curIdx + 1; j < length; j ++)
            {
                char* pnch = chars+charOffs[j];
                strTrie.search(pnch);

                if (strTrie.completeSearch == false)
                    break;

                // matched a word
                if (strTrie.exists())
                {
                    wordEndIdx = j;
                    strTrie.getCurrentNode(endNode);
                    //break; // uncomment if minimum match
                }
            }
        }

        // if matched a synonym
        if (wordEndIdx != size_t(-1))
        {
            retFoundSynonym = true;

            // segment with out synonym
            if (startIdx < curIdx)
            {
                std::vector<UString> segment;
                UString subsegment(pInput+startIdx, curIdx-startIdx);
                //subsegment.displayStringValue(izenelib::util::UString::UTF_8); cout << endl;///
                segment.push_back(subsegment);
                synonymOutput.push_back(segment);
            }

            // segment with synonyms
            std::vector<UString> segment;
            //UString(pInput+curIdx,wordEndIdx+1-curIdx).displayStringValue(izenelib::util::UString::UTF_8); cout <<" [has synonym] ";
            pSynonymContainer_->setSynonym(pSynonymResult_, &endNode);

            size_t cnt, idx = 0;
            set<UString> synonymSet; // avoid duplication
            do
            {
                cnt = pSynonymResult_->getSynonymCount(idx);
                for (size_t off = 0; off < cnt; off++)
                {
                    char * synonymResult = pSynonymResult_->getWord(idx, off);
                    if( synonymResult )
                    {
                        size_t synonymResultLen = strlen(synonymResult);
                        if(synonymResultLen <= term_ustring_buffer_limit_)
                        {
                            synonymResultUstr = synonym_ustring_buffer_;
                            synonymResultUstrLen = UString::toUcs2(synonymEncode_,
                                    synonymResult, synonymResultLen, synonym_ustring_buffer_, term_ustring_buffer_limit_);
                        }

                        UString synonym(synonymResultUstr, synonymResultUstrLen);
                        if(synonymSet.find(synonym) == synonymSet.end()){
                            synonymSet.insert(synonym);
                        }else{
                            continue;
                        }
                        segment.push_back(synonym);
                    }
                }

                idx++;
            }
            while (cnt > 0);
            synonymOutput.push_back(segment);

            curIdx = wordEndIdx+1;
            startIdx = curIdx;
        }
        else
        {
            curIdx++;
        }
    }

    if (!retFoundSynonym)
        return false;

    if (startIdx < curIdx)
    {
        std::vector<UString> segment;
        UString subsegment(pInput+startIdx, curIdx-startIdx);
        //subsegment.displayStringValue(izenelib::util::UString::UTF_8); cout << endl; ///
        segment.push_back(subsegment);
        synonymOutput.push_back(segment);
    }

    return true;
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

                if(false /*bExtractSynonym_, preprocessed*/)
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
                if(false /*bExtractSynonym_, preprocessed*/)
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
                                //cout << "synonym self: "<<string(synonymResult) <<endl;
                                continue;
                            }
                            //cout << "synonym : "<<string(synonymResult) <<endl;

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
