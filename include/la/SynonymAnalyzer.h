/**
 * @file    SynonymAnalyzer.cpp
 * @author  MyungHyun Lee (Kent)
 * @date    Sep 10, 09
 * @details
 */


#ifndef _LA_SYNONYM_ANALYZER_H_
#define _LA_SYNONYM_ANALYZER_H_

#include <Analyzer.h>

#include <am/vsynonym/VSynonym.h>

namespace la
{
    class SynonymAnalyzer : public Analyzer
    {
        public:
            SynonymAnalyzer( )
                : pSynonymContainer_(NULL), pSynonym_(NULL) 
            {
                pSynonymContainer_ = izenelib::am::VSynonymContainer::createObject();
                pSynonymContainer_->setSynonymDelimiter(" ");
                pSynonymContainer_->setWordDelimiter("_");

                pSynonym_ = izenelib::am::VSynonym::createObject();
            }

            virtual int analyze_index( const TermList & input, TermList & output, unsigned char retFlag );
            virtual int analyze_search( const TermList & input, TermList & output, unsigned char retFlag );

        private:
            izenelib::am::VSynonymContainer*   pSynonymContainer_;
            izenelib::am::VSynonym*            pSynonym_;
    };
}



#endif  //_LA_SYNONYM_ANALYZER_H_
