/**
 * @file    CommonLanguageAnalyzer.cpp
 * @author  Kent, Vernkin
 * @date    Nov 23, 2009
 * @details
 *  Common Language Analyzer for Chinese/Japanese/Korean.
 */

#ifndef COMMONLANGUAGEANALYZER_H_
#define COMMONLANGUAGEANALYZER_H_

#include <la/Analyzer.h>
#include <la/SingletonDictionary.h>
#include <la/UpdatableSynonymContainer.h>
#include <la/UpdateDictThread.h>
#include <am/vsynonym/VSynonym.h>

#include <la/stem/Stemmer.h>

#include <util/ustring/UString.h>

#define _CLA_INSERT_INDEX_USTR( term_it, termList, text, wordOffset, pos, morpheme ) \
    term_it = termList.insert( termList.end(), globalNewTerm_ ); \
    term_it->text_ = text; \
    term_it->wordOffset_ = wordOffset; \
    term_it->pos_ = pos; \
    term_it->morpheme_ = morpheme

#define _CLA_INSERT_INDEX_STR( term_it, termList, text, wordOffset, pos, morpheme ) \
    term_it = termList.insert( termList.end(), globalNewTerm_ ); \
    term_it->text_.assign( text, encode_ ); \
    term_it->wordOffset_ = wordOffset; \
    term_it->pos_ = pos; \
    term_it->morpheme_ = morpheme

//#define DEBUG_CLA

namespace la
{
    //class NKoreanAnalyzer;
    /**
     * @brief   Analyzes Korean text using KMA.
     * @details
     *  There are two modes Index/Label
     *  analyze( TermList, UString &, TermList & ); is used to get expanded query string.
     */
    template <class LanguageAction, class BasicSentence>
    class CommonLanguageAnalyzer : public Analyzer
    {
        public:
            int primeCnt_;
            int secCnt_;
            int stemCnt_;
            int cmpCnt_;
            int specCnt_;

            //language-independent attributes
            int flMorp_; // For KMA, it is kmaOrange::FL
            std::string flPOS_; // For KMA, it is "FL"
            int nniMorp_;
            std::string nniPOS_;
            int nnpMorp_;
            std::string nnpPOS_;
            int scMorp_; // For KMA, it is kmaOrange::SC, special character
            izenelib::util::UString::EncodingType encode_;
            /** In the same token, whether each Morpheme shared the same word offset */
            bool bSharedWordOffset_;

            CommonLanguageAnalyzer( const std::string knowledgePath, bool loadModel = true );
            virtual ~CommonLanguageAnalyzer();

            void setIndexMode()
            {
                setExtractEngStem( true );
                lat_->setIndexMode();

                mode_ = INDEXING;
            }
            void setLabelMode()
            {
                setExtractEngStem( false );
                setIndexSynonym( false );
                setSearchSynonym( false );
                setGenerateCompNoun( false );
                lat_->setLabelMode();

                mode_ = LABELING;
            }

            virtual int analyze( UStringHashFunctor * hash, const TermList & input, TermIdList & output, unsigned char retFlag );
            virtual int analyze_index( const TermList & input, TermList & output, unsigned char retFlag );
            virtual int analyze_search( const TermList & input, TermList & output, unsigned char retFlag );

            //------------------ OPTIONS ------------------

            /**
             * @brief   Sets whether to generate compound noun with two adjacent extracted nouns
             *          (only indexing)
             */
            inline void setGenerateCompNoun( bool flag=false )
            { bGenCompNoun_ = flag; }

            /**
             * @brief   Sets the number of NBest to use when extracting terms
             *          (only indexing)
             */
            inline void setNBest( unsigned int num=2 )
            {
                lat_->setNBest( num );
            }

            /**
             * @brief  Sets the lower bound digit limit of the numbers extracted
             */
            inline void setLowDigitBound( unsigned int num=1 )
            {
                lat_->setLowDigitBound( num );
            }

            /**
             * @brief   Whether to combine number dependent noun to extracted term
             */
            inline void setCombineBoundNoun( bool flag=false )
            {
                lat_->setCombineBoundNoun( flag );
            }

            /**
             * @brief   Whether or not to extract stem terms of verb and adj words
             */
            inline void setVerbAdjStems( bool flag=false )
            {
                lat_->setVerbAdjStems( flag );
            }

            /**
             * @brief   Whether to extract Chinese characters additional to their Korean counterpart
             *          (Only for indexing)
             */
            inline void setExtractChinese( bool flag=false )
            {
                bExtractChinese_ = flag;
            }

            /**
             * @brief   Whether or not to extract English stems.
             */
            inline void setExtractEngStem( bool flag=true )
            {
                bExtractEngStem_ = flag;
            }

            /**
             * @brief   Sets whehter synonym is generated for analyze_index
             */
            inline void setIndexSynonym( bool flag=true )
            {
                bIndexSynonym_ = flag;
            }

            /**
             * @brief   Sets whehter synonym is generated for analyze_search
             */
            inline void setSearchSynonym( bool flag=false )
            {
                bSearchSynonym_ = flag;
            }

            /**
             * @brief Whether enable case-sensitive search
             * @param flag default value is true
             */
            virtual void setCaseSensitive( bool flag = true )
            {
                bCaseSensitive_ = flag;
                lat_->setCaseSensitive( flag );
            }

            /**
             * @brief   Set the analysis approach type, only for iCMA
             */
            inline void setAnalysisType( unsigned int type = 2 )
            {
                lat_->setAnalysisType( type );
            }


            // --- SPECIALCHAR ---

            /**
             * @brief   Set special characters to process. The configured characters, of course,
             *          should be passed to KoreanAnalyzer. (normall with "allow" option to Tokenizer)
             */
            void setSpecialChars( const string & chars )
            {
                const char *o = chars.c_str();

                // init special chars table
                memset(specialCharTable_, 0x00, sizeof(char) * 128);

                if (*o)
                {
                    // turn on special chars option.
                    bSpecialChars_ = true;

                    while (*o)
                    {
                        if( (unsigned char)*o < 128 )
                        {
                            specialCharTable_[(unsigned char)*o] = 1;
                        }
                        o++;
                    }
                }
            }

        private:

            void addSynonym(
                    const char * pTerm,
                    const unsigned int wordOffset,
                    TermList & tlist );

            /**
             * @brief   Checks the given term string for Chinese characters, and index them as they are.
             *          KMA will convert the Chinese terms into corresponding Korean terms by default.
             */
            void addChineseTerm(
                    const char * pTerm,
                    const unsigned int wordOffset,
                    TermList & list );

            int getSpecialCharsString( BasicSentence * pEojul, int listi, int counti, std::string & specialStr );

            inline void resetAnalyzer()
            {
                lat_->resetAnalyzer();
            }


            inline void generateCompundNouns( BasicSentence * pE, const string& inputstr, int i, int count,
                    unsigned int wordOffset, TermList& termList )
            {
                unsigned int pos = 0;
                TermList::iterator term_it;
                for ( int j = 0; j < count; j++ )
                {
                    const char* lexicon = pE->getLexicon( i, j );
                    if( strlen(lexicon) == 0 )
                        continue;

                    pos = pE->getPOS(i,j);

                    if( lat_->isAcceptedNoun( pos ) )
                    {
                        for( int k = j+1; k <count; k++ )
                        {
                            pos = pE->getPOS(i,k);
                            if( lat_->isAcceptedNoun( pos ) )
                            {
                                string tstr( lexicon );
                                tstr.append( pE->getLexicon(i,k) );

                                if( tstr == inputstr )
                                    continue;

                                cmpCnt_++;
                                _CLA_INSERT_INDEX_STR( term_it, termList, tstr,
                                        wordOffset, nnpPOS_, nnpMorp_ );
                            }
                        }
                    }
                }
            }

            inline void combineSpecialChar( BasicSentence * pE, int i, int count, TermList& termList )
            {
                TermList::iterator term_it;
                string specialStr;
                int k;
                for( int j = 0; j < count; ++j )
                {
                    unsigned int pos = pE->getPOS(i,j);
                    if( ( pos & scMorp_ ) == pos )
                    {
                        if( (k = getSpecialCharsString(pE, i, j, specialStr)) >= 0 )
                        {
                            _CLA_INSERT_INDEX_STR( term_it, termList, specialStr, k, nniPOS_, nniMorp_ );
                            j = k;
                        }
                    }
                }
            }

        private:
            // new attributes
            LanguageAction*                       lat_;

            // original attributes
            //kmaOrange::WK_Eojul*                pE_;
            //kmaOrange::WK_Analyzer*             pA_;
            //kmaOrange::WK_Eojul*                pEsyn_;     // Eojul for processing synonyms
            //kmaOrange::WK_Analyzer*             pAsyn_;     // Analyzer for processing synonyms
            izenelib::am::VSynonymContainer*      pSynonymContainer_;
            izenelib::am::VSynonym*               pSynonymResult_;
            shared_ptr<UpdatableSynonymContainer> uscSPtr_;

            stem::Stemmer *                     pStemmer_;

            /// @brief Defines the current processing mode
            enum LA_MODE { INDEXING, LABELING };
            LA_MODE mode_;

            // ---- RELATED TO "option" IN LAMANAGER

            /// @brief  Genearte compound noun out of two adjacent extracted noun terms
            /// Default: false
            bool            bGenCompNoun_;


            /// @brief  Can choose the number of possible extractions to generate.
            /// Default: 2
            //unsigned int    nBest_;


            /// @brief  Whether to add Chinese characters additional to their Korean counterpart
            /// Default: false
            bool            bExtractChinese_;     //H

            /// @brief  To extract English stems
            /// Default: true
            bool            bExtractEngStem_;   //S


            // ---- RELATED TO "specialchar" IN LAMANAGER
            /// @brief  Whether to handle special characters.
            /// Adjacent alphabets, numeric characters, and special characters will be concatenated.
            bool bSpecialChars_;

            /// @brief  Table for special characters that should be considered for analyzing.
            char specialCharTable_[128];

            /// @brief  Whether or not to generate synonyms for indexing term generation.
            bool bIndexSynonym_;

            /// @brief  Whether or not to generate synonyms for searach term generation.
            bool bSearchSynonym_;

            static unsigned int sTagSetNoun_;

            /// @brief Don't modify data in globalNewTerm_
            const Term globalNewTerm_;
    };


}

#endif /* COMMONLANGUAGEANALYZER_H_ */
