/**
 * @file    CommonLanguageAnalyzer.cpp
 * @author  Kent, Vernkin
 * @date    Nov 23, 2009
 * @details
 *  Common Language Analyzer for Chinese/Japanese/Korean.
 */
//#define SF1_TIME_CHECK
#include <util/profiler/ProfilerGroup.h>

#include <la/CommonLanguageAnalyzer.h>
#include <la/EnglishUtil.h>
#include <la/UpdateDictThread.h>

//KMA headers
#ifdef USE_WISEKMA
    #include <la/KoreanLanguageAction.h>
    using namespace kmaOrange;
#endif
//end KMA headers

//CMA headers
#ifdef USE_IZENECMA
    #include <la/ChineseLanguageAction.h>
#endif

using namespace izenelib::util;
using namespace std;

namespace la
{
#if __GNUC__ >= 4

#ifdef USE_WISEKMA
//FOR KMA
template CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul>::
    CommonLanguageAnalyzer( const std::string knowledgePath, bool loadModel );
template CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul>::
    ~CommonLanguageAnalyzer();
template int CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul>::
    analyze_index(const TermList & input, TermList & output, unsigned char retFlag );
template int CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul>::
    analyze_search(const TermList & input, TermList & output, unsigned char retFlag );
template void CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul>::
    addSynonym(
                const char * pTerm,
                const unsigned int wordOffset,
                //const unsigned int begin,
                //const unsigned int end,
                //const bool boundStart,
                TermList & tlist );
template void CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul>::
    addChineseTerm(
                const char * pTerm,
                const unsigned int wordOffset,
                TermList & tlist );
template int CommonLanguageAnalyzer<la::KoreanLanguageAction, kmaOrange::WK_Eojul>::
    getSpecialCharsString(
                kmaOrange::WK_Eojul * pEojul, int listi, int counti, string & specialStr );
#endif

#ifdef USE_IZENECMA
//FOR CMA
template CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence>::
    CommonLanguageAnalyzer( const std::string knowledgePath, bool loadModel );
template CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence>::
    ~CommonLanguageAnalyzer();
template int CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence>::
    analyze_index(const TermList & input, TermList & output, unsigned char retFlag );
template int CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence>::
    analyze_search(const TermList & input, TermList & output, unsigned char retFlag );
template void CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence>::
    addSynonym(
                const char * pTerm,
                const unsigned int wordOffset,
                //const unsigned int begin,
                //const unsigned int end,
                //const bool boundStart,
                TermList & tlist );
template void CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence>::
    addChineseTerm(
                const char * pTerm,
                const unsigned int wordOffset,
                TermList & tlist );
template int CommonLanguageAnalyzer<la::ChineseLanguageAction, cma::Sentence>::
    getSpecialCharsString(
                cma::Sentence * pEojul, int listi, int counti, string & specialStr );

#endif // end USE_IZENECMA

#endif // end

template <class LanguageAction, class BasicSentence>
CommonLanguageAnalyzer<LanguageAction, BasicSentence>::CommonLanguageAnalyzer(
        const std::string pKnowledgePath, bool loadModel )
    : Analyzer(),
      lat_( NULL ),
      pSynonymContainer_( NULL ),
      pSynonymResult_( NULL ),
      pStemmer_( NULL )
{
    // ( if possible) remove tailing path separator in the knowledgePath
    string knowledgePath = pKnowledgePath;
    if( knowledgePath.length() > 0 )
    {
        char klpLastChar = knowledgePath[ knowledgePath.length() - 1 ];
        if( klpLastChar == '/' || klpLastChar == '\\' )
            knowledgePath = knowledgePath.substr( 0, knowledgePath.length() - 1 );
    }

    // 0. INIT Language-Independent constant variable
    lat_ = new LanguageAction( knowledgePath, loadModel );
    flMorp_ = lat_->getFLMorp();
    flPOS_ = lat_->getFLPOS();
    nniMorp_ = lat_->getNNIMorp();
    nniPOS_ = lat_->getNNIPOS();
    nnpMorp_ = lat_->getNNPMorp();
    nnpPOS_ = lat_->getNNPPOS();
    scMorp_ = lat_->getSCMorp();
    encode_ = lat_->getEncodeType();
    bSharedWordOffset_ = lat_->isSharedWordOffset();

    // 1. INIT INSTANCES
    primeCnt_ = 0;
    secCnt_ = 0;
    stemCnt_ = 0;
    cmpCnt_ = 0;
    specCnt_ = 0;

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


    // 2. SET DEFAULT SETTINGS


    setGenerateCompNoun();
    setNBest();
    setLowDigitBound();
    setCombineBoundNoun();
    setVerbAdjStems();
    setExtractChinese();
    setExtractEngStem();
    setIndexSynonym();
    setSearchSynonym();
    setCaseSensitive(false);
    bSpecialChars_ = false;

    setIndexMode(); // Index mode is set by default
}

template <class LanguageAction, class BasicSentence>
CommonLanguageAnalyzer<LanguageAction, BasicSentence>::~CommonLanguageAnalyzer()
{
    delete lat_;
    delete pSynonymContainer_;
    delete pSynonymResult_;
    delete pStemmer_;
}
//#define DEBUG_CLA

template <class LanguageAction, class BasicSentence>
int CommonLanguageAnalyzer<LanguageAction, BasicSentence>::analyze_index(
        const TermList & input, TermList & output, unsigned char retFlag )
{
    if( retFlag == 0 )
        return 0;

    // update the synonym if necessary
    if( bIndexSynonym_ )
        pSynonymContainer_ = uscSPtr_->getSynonymContainer();

    TermList tempList;

    TermList::iterator term_it;

    bool analyzePrime = (retFlag & ANALYZE_PRIME_) != 0;
    bool analyzeSecond = (retFlag & ANALYZE_SECOND_) != 0;

    int localOffset = 0;

    for ( TermList::const_iterator it = input.begin(); it != input.end();  ++it )
    {
        string inputstr;
        it->text_.convertString( inputstr, encode_ );

        TermList::iterator prime_it;
        if( analyzePrime )
        {
            prime_it = output.insert( output.end(), *it );
            primeCnt_++;
        }

        if( !analyzeSecond )
            continue;
#ifdef DEBUG_CLA
        cout<<"input stream is "<<inputstr<<endl;
#endif
        BasicSentence* pE = lat_->getBasicSentence( inputstr.c_str() );
#ifdef DEBUG_CLA
        cout<<"end get basic sentence "<<endl;
#endif
        int listSize = pE->getListSize();
        int bestIdx = 0;//pE->getOneBestIndex(); FIXME the one best index maybe not correct
        for ( int i = 0; i < listSize; ++i )
        {
            bool isBestIndex = ( i == bestIdx );
            int count = pE->getCount( i );
            for ( int j = 0; j < count; j++ )
            {
                // if bCaseSensitive_ is true, lexicon is original string
                const char * lexicon = pE->getLexicon( i, j );
                int morpheme = pE->getPOS(i,j);

                if( strlen(lexicon) == 0 )
                    continue;

                // ustring for lexicon and lowerLexicon
                UString lexiconUStr;
                UString lowerLexiconUStr;
                // if bCaseSensitive_ is false, lowerLexicon is equals lexicon
                string lowerLexiconStr;
                const char * lowerLexicon = NULL;

                // decide the offset
                int wOffset = it->wordOffset_;
                if( bSharedWordOffset_ == false )
                {
                    if( isBestIndex && j > 0 )
                    {
                        ++localOffset;
                    }
                    wOffset += localOffset;
                }

                lexiconUStr.assign( lexicon, encode_ );
                bool isFL = (morpheme & flMorp_) == flMorp_;
                #ifdef DEBUG_CLA
                string sss;
                lexiconUStr.convertString(sss, izenelib::util::UString::UTF_8 );
                cout<<"#"<<i<<","<<j<<" get lexicon "<<sss<<endl;
                #endif

                if( bCaseSensitive_ && isFL )
                {
                    lowerLexiconUStr = lexiconUStr;
                    lowerLexiconUStr.toLowerString();
                    lowerLexiconUStr.convertString( lowerLexiconStr, encode_ );
                    lowerLexicon = lowerLexiconStr.c_str();
                }
                else
                {
                    lowerLexicon = lexicon;
                }

#ifdef DEBUG_CLA
                cout<<"To add index Synonym: " << bIndexSynonym_ << ", tempList size: "
                    << tempList.size() << endl;
#endif
                if( bIndexSynonym_ )
                {
                    // search synonyms with lower cases
                    addSynonym( lowerLexicon, wOffset, tempList );
#ifdef DEBUG_CLA
                cout<<"After index Synonym tempList size: " << tempList.size() << endl;
#endif
                }

                //this needs to be set so it includes the POS types that are needed
                if ( pE->isIndexWord(i, j) )
                {
                    string pos = pE->getStrPOS(i,j);

                    if( analyzePrime && count == 1 )
                    {
                        prime_it->pos_ = pos;
                        prime_it->morpheme_ = morpheme;
                    }
                    else
                    {
                        secCnt_++;
                        _CLA_INSERT_INDEX_USTR( term_it, tempList, lexiconUStr, wOffset, pos, morpheme );
                    }

                    if( isFL )
                    {
                        // store the lower case as secondary terms
                        if( bCaseSensitive_ && bContainLower_ && strcmp( lexicon, lowerLexicon ) != 0 )
                        {
                            secCnt_++;
                            _CLA_INSERT_INDEX_USTR( term_it, tempList, lowerLexiconUStr, wOffset, pos, morpheme );
                        }

#ifdef DEBUG_CLA
                        cout<<"To extract English Stemming: " << bExtractEngStem_ << ", tempList size: "
                                << tempList.size() << endl;
#endif

                        if( bExtractEngStem_ )
                        {
                            string stem_term;
                            pStemmer_->stem( lowerLexicon, stem_term );

                            if( strcmp(stem_term.c_str(), lowerLexicon) != 0 )
                            {
                                stemCnt_++;
                                _CLA_INSERT_INDEX_STR( term_it, tempList, stem_term, wOffset, pos, morpheme );
                            }
                        }
#ifdef DEBUG_CLA
                        cout<<"After isFL size: " << tempList.size() << endl;
#endif
                    }

#ifdef DEBUG_CLA
                cout<<"After pE->isIndexWord(i, j) tempList size: " << tempList.size() << endl;
#endif

                } // end if ( pE->isIndexWord(i, j) )
            } // for morpheme list items

#ifdef DEBUG_CLA
            cout<<"To Generate Compound Noun: " << bGenCompNoun_ << ", tempList size: "
                    << tempList.size() << endl;
#endif
            // generate compund nouns
            if( bGenCompNoun_ )
                generateCompundNouns( pE, inputstr, i, count, it->wordOffset_, tempList );
#ifdef DEBUG_CLA
            cout<<"To Combine Special Char: " << bSpecialChars_ << ", tempList size: "
                    << tempList.size() << endl;
#endif
            if( bSpecialChars_ )
                combineSpecialChar( pE, i, count, tempList );

        } // end for( int i = 0; i < listSize; ++i )

#ifdef DEBUG_CLA
        cout<<"To Extract Chinese: " << bExtractChinese_ << ", tempList size: "
            << tempList.size() << endl;
#endif
        if( bExtractChinese_ )
        {
            addChineseTerm( inputstr.c_str(), it->wordOffset_, tempList );
        }

#ifdef DEBUG_CLA
        cout<<"After one inoutput list loop tempList size: " << tempList.size() << endl;
#endif

        if( !tempList.empty() )
        {
            /*for( TermList::iterator itr = tempList.begin(); itr != tempList.end(); ++itr )
            {
                output.push_back( *itr );
            }
            */
            output.splice( output.end(), tempList );
#ifdef DEBUG_CLA
            cout<<"After one inoutput loop output.splice output size: " << output.size() << endl;
#endif
        }

    } // end: for inoutput list
#ifdef DEBUG_CLA
    cout<<" Exit analyze_index. Output Size: " << output.size() << "." << endl;
#endif

    return localOffset;
}

template <class LanguageAction, class BasicSentence>
int CommonLanguageAnalyzer<LanguageAction, BasicSentence>::analyze_search(
        const TermList & input, TermList & output, unsigned char retFlag )
{
    if( retFlag == 0 )
        return 0;

    // update the synonym if necessary
    if( bSearchSynonym_ )
        pSynonymContainer_ = uscSPtr_->getSynonymContainer();

    int i = 0, j = 0, k = 0;
    int count = 0;

    UString uinputstr;
    string inputstr;
    const char * lexicon = NULL;
    // if bCaseSensitive_ is false, lowerLexicon is equals lexicon
    string lowerLexiconStr;
    const char * lowerLexicon = NULL;

    // ustring for lexicon and lowerLexicon
    UString lexiconUStr;
    UString lowerLexiconUStr;

    TermList tempList;
    string pos;
    int morpheme = 0;
    unsigned char level = 0;

    TermList::const_iterator it;
    TermList::iterator term_it;
    TermList::iterator prime_it;
    TermList::iterator synTerm_it;

    int localOffset = 0;

    for ( it = input.begin(); it != input.end();  it++ )
    {
        if( bCaseSensitive_ )
        {
            it->text_.convertString( inputstr, encode_ );
        }
        else
        {
            uinputstr = it->text_;
            uinputstr.toLowerString();
            uinputstr.convertString( inputstr, encode_ );
        }

        if( retFlag & ANALYZE_PRIME_ )
        {
            prime_it = output.insert( output.end(), *it );
            prime_it->stats_ = makeStatBit( Term::OR_BIT, level++ );
        }

        //if( (retFlag & ANALYZE_SECOND_) != retFlag )
        if( (retFlag & ANALYZE_SECOND_) == 0 )
        {
            prime_it->stats_ = makeStatBit( Term::AND_BIT, level-- );
            continue;
        }


        BasicSentence* pE = lat_->getBasicSentence( inputstr.c_str() );
        i = pE->getOneBestIndex();

        count = pE->getCount( i );
        for ( j = 0; j < count; j++ )
        {
            lexicon = pE->getLexicon( i, j );

            #ifdef DEBUG_CLA
            UString uu1( lexicon, encode_ );
            string sss;
            uu1.convertString(sss, izenelib::util::UString::UTF_8 );
            cout<<"#"<<i<<","<<j<<" get lexicon "<<sss<<", pos: "<<pE->getStrPOS(i,j)<<", is index: "<<
            pE->isIndexWord(i, j)<<"."<<endl;
            #endif

            if( strlen(lexicon) == 0 )
                continue;

            // decide the offset
            int wOffset = it->wordOffset_;
            if( bSharedWordOffset_ == false )
            {
                if( j > 0 )
                {
                    ++localOffset;
                }
                wOffset += localOffset;
            }

            //this needs to be set so it includes the POS types that are needed
            if ( pE->isIndexWord(i, j))
            {
                pos = pE->getStrPOS(i,j);
                morpheme = pE->getPOS(i,j);

                bool containUpper = false;

                lexiconUStr.assign( lexicon, encode_ );
                if( bCaseSensitive_ )
                {
                    if( (morpheme & flMorp_) == flMorp_ )
                    {
                        lowerLexiconUStr = lexiconUStr;
                        lowerLexiconUStr.toLowerString();
                        lowerLexiconUStr.convertString( lowerLexiconStr, encode_ );
                        lowerLexicon = lowerLexiconStr.c_str();
                        containUpper = strcmp( lexicon, lowerLexicon) != 0;
                    }
                    else
                    {
                        lowerLexiconUStr = lexiconUStr;
                        lowerLexicon = lexicon;
                    }
                }
                else
                {
                    lowerLexicon = lexicon;
                }


                string stem_term;
                if( (retFlag & ANALYZE_PRIME_) && count == 1 )
                {
                    prime_it->pos_ = pos;
                    prime_it->morpheme_ = morpheme;

                    if( bExtractEngStem_ && ( (morpheme & flMorp_) == flMorp_ ) )
                    {
                        pStemmer_->stem( lowerLexicon, stem_term );
                        // compare with original string
                        if( strcmp(stem_term.c_str(), lexicon) != 0 )
                        {
                            if( containUpper )
                            {
                                if( strcmp(stem_term.c_str(), lowerLexicon) == 0 )
                                    stem_term.clear();
                            }
                            else
                            {
                                prime_it->text_.assign( stem_term, encode_ );
                                stem_term.clear();
                            }
                        }
                        else
                        {
                            stem_term.clear();
                        }
                    }
                    term_it = prime_it;
                    level--;
                }
                else
                {
                    term_it = tempList.insert( tempList.end(), globalNewTerm_ );

                    if( bExtractEngStem_ && ((morpheme & flMorp_) == flMorp_ ) )
                    {
                        pStemmer_->stem( lowerLexicon, stem_term );
                        // compare with original string
                        if( strcmp(stem_term.c_str(), lexicon) != 0 )
                        {
                            if( containUpper )
                            {
                                term_it->text_ = lexiconUStr;
                                if( strcmp(stem_term.c_str(), lowerLexicon) == 0 )
                                    stem_term.clear();
                            }
                            else
                            {
                                term_it->text_.assign( stem_term, encode_ );
                                stem_term.clear();
                            }
                        }
                        else
                        {
                            term_it->text_ = lexiconUStr;
                            stem_term.clear();
                        }
                    }
                    else
                    {
                        term_it->text_.assign( lexicon, encode_ );
                    }
                    term_it->wordOffset_ = wOffset;
                    term_it->pos_ = pos;
                    term_it->morpheme_ = morpheme;

                    term_it->stats_ = makeStatBit( Term::AND_BIT, level );
                }

                if( !stem_term.empty() )
                {
                    //add the lower cases
                    term_it->stats_ = makeStatBit( Term::OR_BIT, ++level );

                    term_it = tempList.insert( tempList.end(), globalNewTerm_ );
                    term_it->text_.assign( stem_term, encode_ );
                    term_it->wordOffset_ = wOffset;
                    term_it->pos_ = pos;
                    term_it->morpheme_ = morpheme;

                    term_it->stats_ = makeStatBit( Term::OR_BIT, level );
                }

                if( bSearchSynonym_ )
                {
                    pSynonymContainer_->searchNgetSynonym( lexicon, pSynonymResult_ );
                    char * synonym = pSynonymResult_->getHeadWord(0);

                    if( synonym != NULL && strcmp(lexicon, synonym) != 0 )
                    {
                        if( stem_term.empty() )
                            term_it->stats_ = makeStatBit( Term::OR_BIT, ++level ); // reset previous stat bit

                        term_it = tempList.insert( tempList.end(), globalNewTerm_ );
                        term_it->text_.assign( synonym, encode_ );
                        term_it->wordOffset_ = wOffset;
                        term_it->pos_ = "?";
                        term_it->morpheme_ = 0;

                        term_it->stats_ = makeStatBit( Term::OR_BIT, level-- ); // set current stat bit
                    }
                }
            }
        }   // count

        if( bSpecialChars_ )
        {
            unsigned int pos = 0;
            string specialStr;

            for( j = 0; j < count; j++ )
            {
                pos = pE->getPOS(i,j);
                if( (pos & scMorp_ ) == pos )
                {
                    if( (k = getSpecialCharsString(pE, i, j, specialStr)) >= 0 )
                    {
                        term_it = tempList.insert( tempList.end(), globalNewTerm_ );

                        term_it->text_.assign( specialStr, encode_ );
                        term_it->wordOffset_ = k;
                        term_it->pos_ = nniPOS_;
                        term_it->morpheme_ = nniMorp_;
                        term_it->stats_ = makeStatBit( Term::AND_BIT, level ); // reset previous stat bit
                    }
                }
            }
            j = k;
        }


        if( !tempList.empty() )
        {
            //if( tempList.size() > 2 )
            //cache_.insertValue( oriText, tempList );
            output.splice( output.end(), tempList );
        }
        else
        {
            if( !(retFlag & ANALYZE_PRIME_) )
            {
                prime_it = output.insert( output.end(), *it );
                prime_it->stats_ = makeStatBit( Term::AND_BIT, level );
            }
        }
        /*
        else if( ( retFlag & ANALYZE_PRIME_ ) && strcmp( lexicon, inputstr.c_str() ) != 0 )
        {
            prime_it = output.insert( output.end(), *it );
            prime_it->stats_ = makeStatBit( Term::AND_BIT, level );
        }
        */

    } // for loop

    return localOffset;
}


template <class LanguageAction, class BasicSentence>
void CommonLanguageAnalyzer<LanguageAction, BasicSentence>::addSynonym(
        const char * pTerm,
        const unsigned int wordOffset,
        //const unsigned int begin,
        //const unsigned int end,
        //const bool boundStart,
        TermList & tlist )
{
    bool found = false;
    bool hasLonger = false;

    unsigned int i = 0, j = 0;
    unsigned int count = 0;

    const char * lexicon = NULL;
    char * synonym = NULL;

    TermList::iterator term_it;     // points to the currently added term

    found = pSynonymContainer_->searchNgetSynonym( pTerm, pSynonymResult_ );
    hasLonger = pSynonymResult_->hasMoreLong();

    {
        //UString tu( term, encode_ );
        //cout << "TERM: "; tu.displayStringValue( UString::UTF_8 ); cout << endl;
    }

    if( found )
    {
        for( int sList = 0; sList < pSynonymResult_->getOverlapCount(); sList++ )
        {
            synonym = pSynonymResult_->getHeadWord( sList );
            if( strcmp(pTerm, synonym) == 0 )
            {
                continue;
            }

            {
#ifdef DEBUG_CLA
                UString tu( synonym, encode_ );
                cout << "SYNONYM: (" << tu.length() << ") ";
                tu.displayStringValue( UString::UTF_8 ); cout << endl;
#endif
            }

            BasicSentence* pEsyn = lat_->getSynBasicSentence( synonym );
            i = pEsyn->getOneBestIndex();

            count = pEsyn->getCount( i );
            for( j = 0; j < count; j++ )
            {
                lexicon = pEsyn->getLexicon( i, j );

                if( strlen(lexicon) == 0 )
                    continue;

                if( pEsyn->isIndexWord(i,j) )
                {
                    _CLA_INSERT_INDEX_STR( term_it, tlist, lexicon,
                            wordOffset, pEsyn->getStrPOS( i, j ), pEsyn->getPOS( i, j ) );
                }
            }
        }
    }

    if( hasLonger )
    {
        // TODO
    }

#ifdef DEBUG_CLA
    cout << "addSynonym output size: " << tlist.size() << "." << endl;
#endif
}

template <class LanguageAction, class BasicSentence>
void CommonLanguageAnalyzer<LanguageAction, BasicSentence>::addChineseTerm(
        const char * pTerm,
        const unsigned int wordOffset,
        TermList & tlist )
{
    TermList::iterator term_it;

    // for string
    const char* h   = pTerm;
    int   len = strlen(pTerm);
    int   sb = -1, eb = -1;

    // for check hanja
    unsigned char hb, lb;

    // for other term
    //char hanja_term[1024+1];
    char * hanja_term = new char[len+1];
    if( hanja_term == NULL )
        return;

    memset( hanja_term, 0, len+1 );

    int i;

    for (i = 0; i < len; i++, h++)
    {
        if (*h & 0x80)
        {
            hb = (unsigned char)*h;
            h++; i++;

            if (!*h) break;

            lb = (unsigned char)*h;

            // is hanja(Chinese)?
            if ((hb >= 0xCA && hb <= 0xFD) && (lb >= 0xA1 && lb <= 0xFE))
            {
                if (sb == -1) sb = i-1;
                eb = i;

                // check next char
                continue;
            }
        }

        // Chinese character check condition
        if (sb >= 0 && (eb > sb))
        {
            strncpy(hanja_term, pTerm + sb, eb-sb+1);
            hanja_term[eb-sb+1] = 0;
            _CLA_INSERT_INDEX_STR( term_it, tlist, hanja_term, wordOffset, flPOS_, flMorp_ );

            sb = -1; eb = -1;
        }
    }

    // Chinese character check condition
    if (sb >= 0 && (eb > sb))
    {
        strncpy(hanja_term, pTerm + sb, eb-sb+1);
        hanja_term[eb-sb+1] = 0;
        _CLA_INSERT_INDEX_STR( term_it, tlist, hanja_term, wordOffset, flPOS_, flMorp_ );

        sb = -1; eb = -1;
    }

    delete[] hanja_term;
}


template <class LanguageAction, class BasicSentence>
int CommonLanguageAnalyzer<LanguageAction, BasicSentence>::getSpecialCharsString(
        BasicSentence * pEojul, int listi, int counti, string & specialStr )
{
    const char *tmpStr;
    int  tmpLen, morphCount;

    tmpStr     = pEojul->getLexicon(listi, counti);
    tmpLen     = strlen(tmpStr);
    morphCount = pEojul->getCount(listi);

    for (int z = 0; z < tmpLen; z++)
    {
        if (!(tmpStr[z] & 0x80) && specialCharTable_[(unsigned char)tmpStr[z]])
        {
            int pi, ni;

            // find start index
            if (counti <= 0) pi = 0;
            else
            {
                for (pi = counti - 1; pi >= 0; pi--)
                {
                    if( !lat_->isScFlSn( pEojul->getPOS(listi, pi) ) )
                    {
                        break;
                    }
                }
                pi++;
            }

            // find end index
            if (counti >= (morphCount-1)) ni = morphCount-1;
            else
            {
                for (ni = counti + 1; ni < morphCount; ni++)
                {
                    if( !lat_->isScFlSn( pEojul->getPOS(listi, pi) ) )
                    {
                        break;
                    }
                }
                ni--;
            }

            // validation
            if (pi == ni) return -1;

            // make term
            specialStr = pEojul->getLexicon(listi, pi);
            //strcpy(pSCString, pEojul->getLexicon(listi, pi));
            for (z = pi + 1; z <= ni; z++)
            {
                //strcat(pSCString, pEojul->getLexicon(listi, z));
                specialStr.append(pEojul->getLexicon(listi, z));
            }
            return ni;
        }
    }
    return -1;
}

}
