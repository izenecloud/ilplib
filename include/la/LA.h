/**
 * @brief   Defines LA class
 * @file    LA.h
 * @date    2009.06.16
 * @author  MyungHyun Lee(Kent), originally zhjay
 * @details
 *  2009.10.25 - The original desing by Jay uses a pipeline approach to combine
 * a number of Analyzers to get a result. This approach is disabled - may be temporarily -
 * due to the fact that it obscures the definition of special, primary, secondary terms.
 * Especially the primary and secondary term. So, the pipeline approach is disabled.
 *
 */

#ifndef _LA_H_
#define _LA_H_

#include <la/common/Term.h>
#include <la/tokenizer/Tokenizer.h>
#include <la/stem/Stemmer.h>
#include <la/util/UStringUtil.h>

#include <la/analyzer/Analyzer.h>
#include <la/analyzer/StemAnalyzer.h>
#include <la/analyzer/NGramAnalyzer.h>
#include <la/analyzer/MatrixAnalyzer.h>
#include <la/analyzer/CharAnalyzer.h>
#include <la/analyzer/TokenAnalyzer.h>
#include <la/analyzer/MultiLanguageAnalyzer.h>
#include <la/analyzer/EnglishAnalyzer.h>
#include <la/analyzer/ChineseAnalyzer.h>
#include <la/analyzer/JapaneseAnalyzer.h>
#include <la/analyzer/KoreanAnalyzer.h>

#include <la/dict/PlainDictionary.h>

#include <boost/smart_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <3rdparty/am/rde_hashmap/hash_map.h>

#include <vector>

namespace la
{

class LA
{
public:

    typedef rde::hash_map<izenelib::util::UCS2Char, bool> PunctsType;

    LA();

    void setAnalyzer( const boost::shared_ptr<Analyzer> & analyzer )
    {
        analyzer_ = analyzer;
    }

    boost::shared_ptr<Analyzer> getAnalyzer()
    {
        return analyzer_;
    }

    /// obsolete
    void processSynonym(TermList& outList)
    {
        if( analyzer_.get() != NULL ) {
            analyzer_->analyzeSynonym(outList, 2);
        }
    }

    void processSynonym( const izenelib::util::UString & inputString,
                  TermList & outList)
    {
        if (analyzer_.get() != NULL) {
            analyzer_->analyzeSynonym(inputString, outList);
        }
    }

    void process( const izenelib::util::UString & inputString,
                  TermList & outList)
    {
        outList.clear();
        if( analyzer_.get() != NULL ) {
            analyzer_->analyze(inputString, outList);
        }
    }

    template<typename IDManagerType>
    void process( IDManagerType* idm,
                  const izenelib::util::UString & inputString,
                  TermIdList & outList )
    {
        outList.clear();
        if( analyzer_.get() != NULL ) {
            analyzer_->analyze( idm, inputString, outList );
        }
    }

    /**
     * Remove Stop Words in the termList
     * \param termList input and output list
     * \param stopDict stop word dictionary
     */
    static void removeStopwords(
        TermList & termList,
        shared_ptr<PlainDictionary>&  stopDict
    );

private:

    shared_ptr<Analyzer>    analyzer_;
};

izenelib::util::UString toExpandedString( const TermList & termList );

}

#endif /* _LA_H_ */
