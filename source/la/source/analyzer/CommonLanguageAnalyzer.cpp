/**
 * @file    CommonLanguageAnalyzer.cpp
 * @author  Kent, Vernkin
 * @date    Nov 23, 2009
 * @details
 *  Common Language Analyzer for Chinese/Japanese/Korean.
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

CommonLanguageAnalyzer::CommonLanguageAnalyzer(
    const std::string pKnowledgePath, bool loadModel )
    : Analyzer(),
    pSynonymContainer_( NULL ),
    pSynonymResult_( NULL ),
    pStemmer_( NULL ),
    bCaseSensitive_(false),
    bContainLower_(false),
    bExtractEngStem_(false),
    bExtractSynonym_(false)
{
    // ( if possible) remove tailing path separator in the knowledgePath
    string knowledgePath = pKnowledgePath;
    if( knowledgePath.length() > 0 )
    {
        char klpLastChar = knowledgePath[ knowledgePath.length() - 1 ];
        if( klpLastChar == '/' || klpLastChar == '\\' )
            knowledgePath = knowledgePath.substr( 0, knowledgePath.length() - 1 );
    }

    pSynonymContainer_ = izenelib::am::VSynonymContainer::createObject();
    if( pSynonymContainer_ == NULL )
    {
    }
    pSynonymResult_ = izenelib::am::VSynonym::createObject();
    if( pSynonymResult_ == NULL )
    {
    }

    string synonymPath = knowledgePath + "/synonym.txt";
    pSynonymContainer_->setSynonymDelimiter(" ");
    pSynonymContainer_->setWordDelimiter("_");
    if( pSynonymContainer_->loadSynonym( synonymPath.c_str() ) != 1 )
    {
        string msg = "Failed to load synonym dictionary from path: ";
        msg += knowledgePath;
        throw std::logic_error( msg );
    }

    // set updatable Synonym Dictionary
    uscSPtr_.reset( new UpdatableSynonymContainer( pSynonymContainer_, synonymPath ) );
    UpdateDictThread::staticUDT.addRelatedDict( synonymPath.c_str(), uscSPtr_ );

    pStemmer_ = new stem::Stemmer();
    pStemmer_->init(stem::STEM_LANG_ENGLISH);

    input_string_buffer_ = new char[input_string_buffer_size_];
    input_lowercase_string_buffer_ = new char[input_string_buffer_size_];
    output_ustring_buffer_ = new UString::CharT[output_ustring_buffer_size_];
    output_lowercase_ustring_buffer_ = new UString::CharT[output_ustring_buffer_size_];
    output_synonym_ustring_buffer_ = new UString::CharT[output_ustring_buffer_size_];
    output_stemming_ustring_buffer_ = new UString::CharT[output_ustring_buffer_size_];
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

    delete input_string_buffer_;
    delete input_lowercase_string_buffer_;
    delete output_ustring_buffer_;
    delete output_lowercase_ustring_buffer_;
    delete output_synonym_ustring_buffer_;
    delete output_stemming_ustring_buffer_;
}

int CommonLanguageAnalyzer::analyze_impl( const Term& input, analyzermode flags, void* data, HookType func )
{

    input.text_.convertString(encode_, input_string_buffer_, input_string_buffer_size_);
    parse(input_string_buffer_, input.wordOffset_);

    while( nextToken() )
    {
        if( len() == 0 )
            continue;

//            {
//            UString foo(token(), encode_);
//            string bar;
//            foo.convertString(bar, UString::UTF_8);
//            cout << "(" << bar << ") --<> " << needIndex() << "  " << hex << morpheme() << dec << endl;
//            }
        if( needIndex() )
        {
            if(isSpecialChar())
            {
                func( data, PLACE_HOLDER.c_str(), PLACE_HOLDER.length(), offset());
                continue;
            }

            UString::CharT* termUstr = output_ustring_buffer_;
            size_t termUstrLen = UString::toUcs2(encode_, token(), len(), output_ustring_buffer_, output_ustring_buffer_size_);
            const char* synonymInput = token();

            // foreign language, e.g. English
            if( isFL() )
            {
                UString::CharT* lowercaseTermUstr = output_lowercase_ustring_buffer_;
                bool lowercaseIsDifferent = UString::toLowerString(termUstr, termUstrLen,
                                            output_lowercase_ustring_buffer_, output_ustring_buffer_size_);

                char* lowercaseTerm = input_lowercase_string_buffer_;
                UString::convertString(encode_, lowercaseTermUstr, termUstrLen, input_lowercase_string_buffer_, input_string_buffer_size_);

                if(bCaseSensitive_)
                {
                    func( data,  termUstr, termUstrLen, offset() );
                    if(bContainLower_ & lowercaseIsDifferent)
                    {
                        func( data, lowercaseTermUstr, termUstrLen, offset());
                    }
                }
                else
                {
                    func( data, lowercaseTermUstr, termUstrLen, offset());
                }

                if(bExtractEngStem_)
                {
                    /// TODO: write a UCS2 based stemmer
                    string stem_term;
                    pStemmer_->stem( lowercaseTerm, stem_term );
                    if( strcmp(stem_term.c_str(), lowercaseTerm) != 0 )
                    {
                        UString::CharT* stemmingTermUstr = output_stemming_ustring_buffer_;
                        size_t stemmingTermUstrSize = UString::toUcs2(encode_, stem_term.c_str(), stem_term.size(),
                                                      output_stemming_ustring_buffer_, output_ustring_buffer_size_);
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
                func( data, termUstr, termUstrLen, offset());
            }

            if(bExtractSynonym_)
            {
                pSynonymContainer_ = uscSPtr_->getSynonymContainer();
                pSynonymContainer_->searchNgetSynonym( synonymInput, pSynonymResult_ );
                char * synonymResult = pSynonymResult_->getHeadWord(0);
                if( synonymResult )
                {
                    cout << to_utf8(synonymInput,encode_) << "--<>" <<
                         to_utf8(synonymResult, encode_) << "," <<
                         to_utf8(pSynonymResult_->getHeadWord(1), encode_) << "," << endl;

                    size_t synonymResultLen = strlen(synonymResult);
                    if(synonymResultLen <= output_ustring_buffer_size_)
                    {
                        UString::CharT * synonymResultUstr = output_synonym_ustring_buffer_;
                        size_t synonymResultUstrLen = UString::toUcs2(encode_, synonymResult, synonymResultLen,
                                                      output_synonym_ustring_buffer_, output_ustring_buffer_size_);
                        func( data, synonymResultUstr, synonymResultUstrLen, offset());
                    }
                }
            }
        }
    }
    return offset();
}

template<typename IDManagerType>
void CommonLanguageAnalyzer::appendTermIdList( void* data,
        const UString::CharT* text, const size_t len, const int offset )
{
    TermIdList * output = ((std::pair<TermIdList*, IDManagerType*>* ) data)->first;
    IDManagerType * idm = ((std::pair<TermIdList*, IDManagerType*>* ) data)->second;

    output->add(idm, text, len, offset);
}

void CommonLanguageAnalyzer::appendTermList( void* data,
        const UString::CharT* text, const size_t len, const int offset )
{
    TermList * output = (TermList *) data;
    output->add( UString(text, len), offset );
}


int CommonLanguageAnalyzer::analyze_( const Term & input, TermList & output, analyzermode flags)
{
    return analyze_impl(input, flags, &output, &CommonLanguageAnalyzer::appendTermList);
}

template <typename IDManagerType>
int CommonLanguageAnalyzer::analyze_(
    IDManagerType* idm, const Term & input, TermIdList & output, analyzermode flags)
{
    pair<TermIdList*, IDManagerType*> env = make_pair(&output, idm);
    return analyze_impl(input, flags, &env, &CommonLanguageAnalyzer::appendTermIdList<IDManagerType>);
}

}
//
//static char ustring_convert_buffer1[4096*4];
//
//static char ustring_convert_buffer[4096];
//
//
//#if __GNUC__ >= 4
//
//#ifdef USE_WISEKMA
////FOR KMA
//template CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul>::
//CommonLanguageAnalyzer( const std::string knowledgePath, bool loadModel );
//template CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul>::
//~CommonLanguageAnalyzer();
////template int CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul>::
////analyze(IDManager* idm, const TermList & input, TermIdList & output, analyzermode retFlag );
//#endif
//
//#ifdef USE_IZENECMA
////FOR CMA
//template CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence>::
//CommonLanguageAnalyzer( const std::string knowledgePath, bool loadModel );
//template CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence>::
//~CommonLanguageAnalyzer();
////template int CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence>::
////analyze(IDManager* idm, const TermList & input, TermIdList & output, analyzermode retFlag );
//#endif // end USE_IZENECMA
//
//#endif // end
//

//
//template <class LanguageAction, class BasicSentence>
//int CommonLanguageAnalyzer<LanguageAction, BasicSentence>::analyze_index(
//    const TermList & input, TermList & output, unsigned char retFlag )
//{
//    if( retFlag == 0 )
//        return 0;
//
//    // update the synonym if necessary
//    if( bIndexSynonym_ )
//        pSynonymContainer_ = uscSPtr_->getSynonymContainer();
//
//    TermList tempList;
//
//    TermList::iterator term_it;
//
//    bool analyzePrime = (retFlag & ANALYZE_PRIME_) != 0;
//    bool analyzeSecond = (retFlag & ANALYZE_SECOND_) != 0;
//
//    int localOffset = 0;
//
//    for ( TermList::const_iterator it = input.begin(); it != input.end();  ++it )
//    {
//        string inputstr;
//        it->text_.convertString( inputstr, encode_ );
//
//        TermList::iterator prime_it;
//        if( analyzePrime )
//        {
//            prime_it = output.insert( output.end(), *it );
//            prime_it->wordOffset_ += localOffset;
//            primeCnt_++;
//        }
//
//        if( !analyzeSecond )
//            continue;
//#ifdef DEBUG_CLA
//        cout<<"input stream is "<<inputstr<<endl;
//#endif
//        BasicSentence* pE = lat_->getBasicSentence( inputstr.c_str() );
//#ifdef DEBUG_CLA
//        cout<<"end get basic sentence "<<endl;
//#endif
//        int listSize = pE->getListSize();
//        int bestIdx = 0;//pE->getOneBestIndex(); FIXME the one best index maybe not correct
//        for ( int i = 0; i < listSize; ++i )
//        {
//            bool isBestIndex = ( i == bestIdx );
//            int count = pE->getCount( i );
//            for ( int j = 0; j < count; j++ )
//            {
//                // if bCaseSensitive_ is true, lexicon is original string
//                const char * lexicon = pE->getLexicon( i, j );
//                int morpheme = pE->getPOS(i,j);
//
//                if( strlen(lexicon) == 0 )
//                    continue;
//
//                // ustring for lexicon and lowerLexicon
//                UString lexiconUStr;
//                UString lowerLexiconUStr;
//                // if bCaseSensitive_ is false, lowerLexicon is equals lexicon
//                string lowerLexiconStr;
//                const char * lowerLexicon = NULL;
//
//                // decide the offset
//                int wOffset = it->wordOffset_;
//                if( bSharedWordOffset_ == false )
//                {
//                    if( isBestIndex && j > 0 )
//                    {
//                        ++localOffset;
//                    }
//                    wOffset += localOffset;
//                }
//
//                /// TODO major cost, about 10%
//                lexiconUStr.assign( lexicon, encode_ );
//                bool isFL = (morpheme & flMorp_) == flMorp_;
//#ifdef DEBUG_CLA
//                string sss;
//                lexiconUStr.convertString(sss, izenelib::util::UString::UTF_8 );
//                cout<<"#"<<i<<","<<j<<" get lexicon "<<sss<<endl;
//#endif
//
//                if( bCaseSensitive_ && isFL )
//                {
//                    lowerLexiconUStr = lexiconUStr;
//                    lowerLexiconUStr.toLowerString();
//                    lowerLexiconUStr.convertString( lowerLexiconStr, encode_ );
//                    lowerLexicon = lowerLexiconStr.c_str();
//                }
//                else
//                {
//                    lowerLexicon = lexicon;
//                }
//
//#ifdef DEBUG_CLA
//                cout<<"To add index Synonym: " << bIndexSynonym_ << ", tempList size: "
//                    << tempList.size() << endl;
//#endif
//                if( bIndexSynonym_ )
//                {
//                    // search synonyms with lower cases
//                    addSynonym( lowerLexicon, wOffset, tempList );
//#ifdef DEBUG_CLA
//                    cout<<"After index Synonym tempList size: " << tempList.size() << endl;
//#endif
//                }
//
//                //this needs to be set so it includes the POS types that are needed
//                if ( pE->isIndexWord(i, j) )
//                {
//                    string pos = pE->getStrPOS(i,j);
//
//                    if( analyzePrime && count == 1 )
//                    {
//                        prime_it->pos_ = pos;
//                        prime_it->morpheme_ = morpheme;
//                    }
//                    else
//                    {
//                        secCnt_++;
//                        /// TODO major cost about 10%
//                        _CLA_INSERT_INDEX_USTR( term_it, tempList, lexiconUStr, wOffset, pos, morpheme );
//                    }
//
//                    if( isFL )
//                    {
//                        // store the lower case as secondary terms
//                        if( bCaseSensitive_ && bContainLower_ && strcmp( lexicon, lowerLexicon ) != 0 )
//                        {
//                            secCnt_++;
//                            _CLA_INSERT_INDEX_USTR( term_it, tempList, lowerLexiconUStr, wOffset, pos, morpheme );
//                        }
//
//#ifdef DEBUG_CLA
//                        cout<<"To extract English Stemming: " << bExtractEngStem_ << ", tempList size: "
//                            << tempList.size() << endl;
//#endif
//
//                        if( bExtractEngStem_ )
//                        {
//                            string stem_term;
//                            pStemmer_->stem( lowerLexicon, stem_term );
//
//                            if( strcmp(stem_term.c_str(), lowerLexicon) != 0 )
//                            {
//                                stemCnt_++;
//                                _CLA_INSERT_INDEX_STR( term_it, tempList, stem_term, wOffset, pos, morpheme );
//                            }
//                        }
//#ifdef DEBUG_CLA
//                        cout<<"After isFL size: " << tempList.size() << endl;
//#endif
//                    }
//
//#ifdef DEBUG_CLA
//                    cout<<"After pE->isIndexWord(i, j) tempList size: " << tempList.size() << endl;
//#endif
//
//                } // end if ( pE->isIndexWord(i, j) )
//            } // for morpheme list items
//
//#ifdef DEBUG_CLA
//            cout<<"To Generate Compound Noun: " << bGenCompNoun_ << ", tempList size: "
//                << tempList.size() << endl;
//#endif
//            // generate compund nouns
//            if( bGenCompNoun_ )
//                generateCompundNouns( pE, inputstr, i, count, it->wordOffset_, tempList );
//#ifdef DEBUG_CLA
//            cout<<"To Combine Special Char: " << bSpecialChars_ << ", tempList size: "
//                << tempList.size() << endl;
//#endif
//            if( bSpecialChars_ )
//                combineSpecialChar( pE, i, count, tempList );
//
//        } // end for( int i = 0; i < listSize; ++i )
//
//#ifdef DEBUG_CLA
//        cout<<"To Extract Chinese: " << bExtractChinese_ << ", tempList size: "
//            << tempList.size() << endl;
//#endif
//        if( bExtractChinese_ )
//        {
//            addChineseTerm( inputstr.c_str(), it->wordOffset_, tempList );
//        }
//
//#ifdef DEBUG_CLA
//        cout<<"After one inoutput list loop tempList size: " << tempList.size() << endl;
//#endif
//
//        if( !tempList.empty() )
//        {
//            /*for( TermList::iterator itr = tempList.begin(); itr != tempList.end(); ++itr )
//            {
//                output.push_back( *itr );
//            }
//            */
////            output.splice( output.end(), tempList );
//#ifdef DEBUG_CLA
//            cout<<"After one inoutput loop output.splice output size: " << output.size() << endl;
//#endif
//        }
//
//    } // end: for inoutput list
//#ifdef DEBUG_CLA
//    cout<<" Exit analyze_index. Output Size: " << output.size() << "." << endl;
//#endif
//
//    return localOffset;
//}
//
//template <class LanguageAction, class BasicSentence>
//int CommonLanguageAnalyzer<LanguageAction, BasicSentence>::analyze_search(
//    const TermList & input, TermList & output, unsigned char retFlag )
//{
//    if( retFlag == 0 )
//        return 0;
//
//    // update the synonym if necessary
//    if( bSearchSynonym_ )
//        pSynonymContainer_ = uscSPtr_->getSynonymContainer();
//
//    int i = 0, j = 0, k = 0;
//    int count = 0;
//
//    UString uinputstr;
//    string inputstr;
//    const char * lexicon = NULL;
//    // if bCaseSensitive_ is false, lowerLexicon is equals lexicon
//    string lowerLexiconStr;
//    const char * lowerLexicon = NULL;
//
//    // ustring for lexicon and lowerLexicon
//    UString lexiconUStr;
//    UString lowerLexiconUStr;
//
//    TermList tempList;
//    string pos;
//    int morpheme = 0;
//    unsigned char level = 0;
//
//    TermList::const_iterator it;
//    TermList::iterator term_it;
//    TermList::iterator prime_it;
//    TermList::iterator synTerm_it;
//
//    int localOffset = 0;
//
//    for ( it = input.begin(); it != input.end();  it++ )
//    {
//        if( bCaseSensitive_ )
//        {
//            it->text_.convertString( inputstr, encode_ );
//        }
//        else
//        {
//            uinputstr = it->text_;
//            uinputstr.toLowerString();
//            uinputstr.convertString( inputstr, encode_ );
//        }
//
//        if( retFlag & ANALYZE_PRIME_ )
//        {
//            prime_it = output.insert( output.end(), *it );
//            prime_it->wordOffset_ += localOffset;
//            prime_it->stats_ = makeStatBit( Term::OR_BIT, level++ );
//        }
//
//        //if( (retFlag & ANALYZE_SECOND_) != retFlag )
//        if( (retFlag & ANALYZE_SECOND_) == 0 )
//        {
//            prime_it->stats_ = makeStatBit( Term::AND_BIT, level-- );
//            continue;
//        }
//
//
//        BasicSentence* pE = lat_->getBasicSentence( inputstr.c_str() );
//        i = pE->getOneBestIndex();
//
//        count = pE->getCount( i );
//        for ( j = 0; j < count; j++ )
//        {
//            lexicon = pE->getLexicon( i, j );
//
//#ifdef DEBUG_CLA
//            UString uu1( lexicon, encode_ );
//            string sss;
//            uu1.convertString(sss, izenelib::util::UString::UTF_8 );
//            cout<<"#"<<i<<","<<j<<" get lexicon "<<sss<<", pos: "<<pE->getStrPOS(i,j)<<", is index: "<<
//                pE->isIndexWord(i, j)<<"."<<endl;
//#endif
//
//            if( strlen(lexicon) == 0 )
//                continue;
//
//            // decide the offset
//            int wOffset = it->wordOffset_;
//            if( bSharedWordOffset_ == false )
//            {
//                if( j > 0 )
//                {
//                    ++localOffset;
//                }
//                wOffset += localOffset;
//            }
//
//            //this needs to be set so it includes the POS types that are needed
//            if ( pE->isIndexWord(i, j))
//            {
//                pos = pE->getStrPOS(i,j);
//                morpheme = pE->getPOS(i,j);
//
//                bool containUpper = false;
//
//                lexiconUStr.assign( lexicon, encode_ );
//                if( bCaseSensitive_ )
//                {
//                    if( (morpheme & flMorp_) == flMorp_ )
//                    {
//                        lowerLexiconUStr = lexiconUStr;
//                        lowerLexiconUStr.toLowerString();
//                        lowerLexiconUStr.convertString( lowerLexiconStr, encode_ );
//                        lowerLexicon = lowerLexiconStr.c_str();
//                        containUpper = strcmp( lexicon, lowerLexicon) != 0;
//                    }
//                    else
//                    {
//                        lowerLexiconUStr = lexiconUStr;
//                        lowerLexicon = lexicon;
//                    }
//                }
//                else
//                {
//                    lowerLexicon = lexicon;
//                }
//
//
//                string stem_term;
//                if( (retFlag & ANALYZE_PRIME_) && count == 1 )
//                {
//                    prime_it->pos_ = pos;
//                    prime_it->morpheme_ = morpheme;
//
//                    if( bExtractEngStem_ && ( (morpheme & flMorp_) == flMorp_ ) )
//                    {
//                        pStemmer_->stem( lowerLexicon, stem_term );
//                        // compare with original string
//                        if( strcmp(stem_term.c_str(), lexicon) != 0 )
//                        {
//                            if( containUpper )
//                            {
//                                if( strcmp(stem_term.c_str(), lowerLexicon) == 0 )
//                                    stem_term.clear();
//                            }
//                            else
//                            {
//                                prime_it->text_.assign( stem_term, encode_ );
//                                stem_term.clear();
//                            }
//                        }
//                        else
//                        {
//                            stem_term.clear();
//                        }
//                    }
//                    term_it = prime_it;
//                    level--;
//                }
//                else
//                {
//                    term_it = tempList.insert( tempList.end(), globalNewTerm_ );
//
//                    if( bExtractEngStem_ && ((morpheme & flMorp_) == flMorp_ ) )
//                    {
//                        pStemmer_->stem( lowerLexicon, stem_term );
//                        // compare with original string
//                        if( strcmp(stem_term.c_str(), lexicon) != 0 )
//                        {
//                            if( containUpper )
//                            {
//                                term_it->text_ = lexiconUStr;
//                                if( strcmp(stem_term.c_str(), lowerLexicon) == 0 )
//                                    stem_term.clear();
//                            }
//                            else
//                            {
//                                term_it->text_.assign( stem_term, encode_ );
//                                stem_term.clear();
//                            }
//                        }
//                        else
//                        {
//                            term_it->text_ = lexiconUStr;
//                            stem_term.clear();
//                        }
//                    }
//                    else
//                    {
//                        term_it->text_.assign( lexicon, encode_ );
//                    }
//                    term_it->wordOffset_ = wOffset;
//                    term_it->pos_ = pos;
//                    term_it->morpheme_ = morpheme;
//
//                    term_it->stats_ = makeStatBit( Term::AND_BIT, level );
//                }
//
//                if( !stem_term.empty() )
//                {
//                    //add the lower cases
//                    term_it->stats_ = makeStatBit( Term::OR_BIT, ++level );
//
//                    term_it = tempList.insert( tempList.end(), globalNewTerm_ );
//                    term_it->text_.assign( stem_term, encode_ );
//                    term_it->wordOffset_ = wOffset;
//                    term_it->pos_ = pos;
//                    term_it->morpheme_ = morpheme;
//
//                    term_it->stats_ = makeStatBit( Term::OR_BIT, level );
//                }
//
//                if( bSearchSynonym_ )
//                {
//                    pSynonymContainer_->searchNgetSynonym( lexicon, pSynonymResult_ );
//                    char * synonym = pSynonymResult_->getHeadWord(0);
//
//                    if( synonym != NULL && strcmp(lexicon, synonym) != 0 )
//                    {
//                        if( stem_term.empty() )
//                            term_it->stats_ = makeStatBit( Term::OR_BIT, ++level ); // reset previous stat bit
//
//                        term_it = tempList.insert( tempList.end(), globalNewTerm_ );
//                        term_it->text_.assign( synonym, encode_ );
//                        term_it->wordOffset_ = wOffset;
//                        term_it->pos_ = "?";
//                        term_it->morpheme_ = 0;
//
//                        term_it->stats_ = makeStatBit( Term::OR_BIT, level-- ); // set current stat bit
//                    }
//                }
//            }
//        }   // count
//
//        if( bSpecialChars_ )
//        {
//            unsigned int pos = 0;
//            string specialStr;
//
//            for( j = 0; j < count; j++ )
//            {
//                pos = pE->getPOS(i,j);
//                if( (pos & scMorp_ ) == pos )
//                {
//                    if( (k = getSpecialCharsString(pE, i, j, specialStr)) >= 0 )
//                    {
//                        term_it = tempList.insert( tempList.end(), globalNewTerm_ );
//
//                        term_it->text_.assign( specialStr, encode_ );
//                        term_it->wordOffset_ = k;
//                        term_it->pos_ = nniPOS_;
//                        term_it->morpheme_ = nniMorp_;
//                        term_it->stats_ = makeStatBit( Term::AND_BIT, level ); // reset previous stat bit
//                    }
//                }
//            }
//            j = k;
//        }
//
//
//        if( !tempList.empty() )
//        {
//            //if( tempList.size() > 2 )
//            //cache_.insertValue( oriText, tempList );
////            output.splice( output.end(), tempList );
//        }
//        else
//        {
//            if( !(retFlag & ANALYZE_PRIME_) )
//            {
//                prime_it = output.insert( output.end(), *it );
//                prime_it->wordOffset_ += localOffset;
//                prime_it->stats_ = makeStatBit( Term::AND_BIT, level );
//            }
//        }
//        /*
//        else if( ( retFlag & ANALYZE_PRIME_ ) && strcmp( lexicon, inputstr.c_str() ) != 0 )
//        {
//            prime_it = output.insert( output.end(), *it );
//            prime_it->stats_ = makeStatBit( Term::AND_BIT, level );
//        }
//        */
//
//    } // for loop
//
//    return localOffset;
//}
//
//
//}
