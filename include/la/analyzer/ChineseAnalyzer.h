#ifndef _CHINESE_ANALYZER_H_
#define _CHINESE_ANALYZER_H_

#include <la/analyzer/Analyzer.h>
#include <la/dict/SingletonDictionary.h>
#include <la/dict/UpdatableSynonymContainer.h>
#include <la/dict/UpdateDictThread.h>
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

namespace la
{
    template<typename IDManagerType>
    class ChineseAnalyzer : public Analyzer
    {
        public:

            enum LA_MODE { INDEXING, LABELING };

            ChineseAnalyzer( const std::string knowledgePath, bool loadModel = true, LA_MODE mode);

            virtual ~ChineseAnalyzer();

//            int primeCnt_;
//            int secCnt_;
//            int stemCnt_;
//            int cmpCnt_;
//            int specCnt_;
//
//            //language-independent attributes
//            int flMorp_; // For KMA, it is kmaOrange::FL
//            std::string flPOS_; // For KMA, it is "FL"
//            int nniMorp_;
//            std::string nniPOS_;
//            int nnpMorp_;
//            std::string nnpPOS_;
//            int scMorp_; // For KMA, it is kmaOrange::SC, special character

//
//            /** In the same token, whether each Morpheme shared the same word offset */
//            bool bSharedWordOffset_;

            void setIDManager( IDManagerType* pIDManager) {
                pIDManager_ = pIDManager;
            }

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

            virtual int analyze(izenelib::ir::idmanager::IDManager* idm, const Term & input, TermIdList & output, unsigned char retFlag );

            //------------------ OPTIONS ------------------

            /**
             * @brief   Sets the number of NBest to use when extracting terms
             *          (only indexing)
             */
            inline void setNBest( unsigned int num=2 )
            {
                lat_->setNBest( num );
            }


            /**
             * @brief   Whether or not to extract English stems.
             */
            inline void setExtractEngStem( bool flag=true )
            {
                bExtractEngStem_ = flag;
            }

            /**
             * @brief Whether enable case-sensitive search
             * @param flag default value is true
             */
            virtual void setCaseSensitive( bool flag = true )
            {
                bCaseSensitive_ = flag;
            }

            /**
             * @brief   Set the analysis approach type
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

            int getSpecialCharsString( BasicSentence * pEojul, int listi, int counti, std::string & specialStr );

            inline void resetAnalyzer()
            {
                lat_->resetAnalyzer();
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
            izenelib::am::VSynonymContainer*      pSynonymContainer_;
            izenelib::am::VSynonym*               pSynonymResult_;
            shared_ptr<UpdatableSynonymContainer> uscSPtr_;

            stem::Stemmer *                     pStemmer_;

            izenelib::util::UString::EncodingType encode_;

            IDManagerType* pIDManager_;

            /// @brief Defines the current processing mode
            LA_MODE mode_;

            // ---- RELATED TO "option" IN LAMANAGER


            /// @brief  Can choose the number of possible extractions to generate.
            /// Default: 2
            //unsigned int    nBest_;


            /// @brief  To extract English stems
            /// Default: true
            bool            bExtractEngStem_;   //S


            // ---- RELATED TO "specialchar" IN LAMANAGER
            /// @brief  Whether to handle special characters.
            /// Adjacent alphabets, numeric characters, and special characters will be concatenated.
            bool bSpecialChars_;

            /// @brief  Table for special characters that should be considered for analyzing.
            char specialCharTable_[128];

            static unsigned int sTagSetNoun_;

    };


}

#endif /* COMMONLANGUAGEANALYZER_H_ */
