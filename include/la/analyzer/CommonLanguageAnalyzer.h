/**
 * @file    CommonLanguageAnalyzer.cpp
 * @author  Kent, Vernkin
 * @date    Nov 23, 2009
 * @details
 *  Common Language Analyzer for Chinese/Japanese/Korean.
 */

#ifndef COMMONLANGUAGEANALYZER_H_
#define COMMONLANGUAGEANALYZER_H_

#include <la/analyzer/Analyzer.h>
#include <la/dict/SingletonDictionary.h>
#include <la/dict/UpdatableSynonymContainer.h>
#include <la/dict/UpdateDictThread.h>
#include <am/vsynonym/VSynonym.h>

#include <la/stem/Stemmer.h>

#include <util/ustring/UString.h>

namespace la
{
    template <class LanguageAction, class BasicSentence>
    class CommonLanguageAnalyzer : public Analyzer
    {
        public:
            //language-independent attributes
            int flMorp_; // For KMA, it is kmaOrange::FL
            std::string flPOS_; // For KMA, it is "FL"
            int nniMorp_;
            std::string nniPOS_;
            int nnpMorp_;
            std::string nnpPOS_;
            izenelib::util::UString::EncodingType encode_;

            /** In the same token, whether each Morpheme shared the same word offset */
            bool bSharedWordOffset_;

            CommonLanguageAnalyzer( const std::string knowledgePath, bool loadModel = true );

            virtual ~CommonLanguageAnalyzer();

            void setIndexMode()
            {
//                setExtractEngStem( true );
                lat_->setIndexMode();

//                mode_ = INDEXING;
            }
            void setLabelMode()
            {
//                setExtractEngStem( false );
//                setIndexSynonym( false );
//                setSearchSynonym( false );
//                setGenerateCompNoun( false );
                lat_->setLabelMode();

//                mode_ = LABELING;
            }

            virtual int analyze(izenelib::ir::idmanager::IDManager* idm,
                const Term & input, TermIdList & output, analyzermode flags);

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

        protected:

            inline void resetAnalyzer()
            {
                lat_->resetAnalyzer();
            }

        protected:

            LanguageAction*                       lat_;

            izenelib::am::VSynonymContainer*      pSynonymContainer_;
            izenelib::am::VSynonym*               pSynonymResult_;
            shared_ptr<UpdatableSynonymContainer> uscSPtr_;

            stem::Stemmer *                     pStemmer_;
    };
}

#endif /* COMMONLANGUAGEANALYZER_H_ */
