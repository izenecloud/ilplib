/**
 * @brief   Defines Analyzerclass
 * @file    Analyzer.h
 * @author  zhjay, MyungHyun Lee (Kent)
 * @date    2009.06.10
 * @details
 *  2009.08.02 - Adding new interfaces
 */

/**
 * @brief   Embed IDManager in LA to accelerate the speed of indexing,
 *          Merging different interfaces into one.
 * @author  Wei
 * @date    2010.08.23
 */

#ifndef _LA_ANALYZER_H_
#define _LA_ANALYZER_H_

#include <la/common/Term.h>
#include <la/tokenizer/Tokenizer.h>

#include <util/ustring/UString.h>
#include <la/util/UStringUtil.h>

namespace la
{

class MultiLanguageAnalyzer;

///
/// \brief interface of Analyzer
/// This class analyze terms according to the specific types of analyzer
///
class Analyzer
{

public:

    Analyzer() : bExtractSpecialChar_(true), bConvertToPlaceHolder_(true) {}

    virtual ~Analyzer() {}

    void setInnerAnalyzer(boost::shared_ptr<la::Analyzer>& analyzer)
    {
        innerAnalyzer_ = analyzer;
    }

    void setTokenizerConfig( const TokenizeConfig & tokenConfig )
    {
        tokenizer_.setConfig( tokenConfig );
    }

    void setExtractSpecialChar(bool extractSpecialChar, bool convertToPlaceHolder = true)
    {
        bExtractSpecialChar_ = extractSpecialChar;
        bConvertToPlaceHolder_ = convertToPlaceHolder;
    }

    virtual void analyzeSynonym(TermList& output, size_t n)
    {
    }

    int analyze(const Term & input, TermList & output)
    {
        void* array[2] = {&output, this};
        return analyze_impl(input, &array, &Analyzer::appendTermList);
    }

    template <typename IDManagerType>
    int analyze( IDManagerType* idm, const Term & input, TermIdList & output)
    {
        void* array[3] = {&output, idm, this};
        return analyze_impl(input, &array, &Analyzer::appendTermIdList<IDManagerType>);
    }

protected:

    friend class MultiLanguageAnalyzer;

    typedef void (*HookType) (
        void* data,
        const UString::CharT* text,
        const size_t len,
        const unsigned int offset,
        const char * pos,
        const unsigned char andOrBit,
        const unsigned int level,
        const bool isSpecialChar );

    template<typename IDManagerType>
    static void appendTermIdList(
        void* data,
        const UString::CharT* text,
        const size_t len,
        const unsigned int offset,
        const char * pos,
        const unsigned char andOrBit,
        const unsigned int level,
        const bool isSpecialChar )
    {
        void** parameters = (void**)data;
        TermIdList * output = (TermIdList*) parameters[0];
        IDManagerType * idm = (IDManagerType*) parameters[1];
        Analyzer * analyzer = (Analyzer*) parameters[2];

        if(isSpecialChar && !analyzer->bExtractSpecialChar_)
            return;
        if(isSpecialChar && analyzer->bConvertToPlaceHolder_) {
            unsigned int termId;
            idm->getTermIdByTermString(PLACE_HOLDER.c_str(), PLACE_HOLDER.length(), termId);
            if( output->empty() == true || output->back().termid_ != termId )
                output->add(termId, offset);
        } else {
            output->add(idm, text, len, offset);
        }

//        UString utext(text, 0, len);
//        cout << la::to_utf8(utext) << " " << output->back().termid_ << endl;
    }

    static void appendTermList(
        void* data,
        const UString::CharT* text,
        const size_t len,
        const unsigned int offset,
        const char * pos,
        const unsigned char andOrBit,
        const unsigned int level,
        const bool isSpecialChar )
    {
        void** parameters = (void**)data;
        TermList * output = (TermList*) parameters[0];
        Analyzer * analyzer = (Analyzer*) parameters[1];

        if(isSpecialChar && !analyzer->bExtractSpecialChar_)
            return;
        if(isSpecialChar && analyzer->bConvertToPlaceHolder_)
        {
            if( output->empty() == true || output->back().text_.compare(PLACE_HOLDER) != 0)
                output->add(PLACE_HOLDER.c_str(), PLACE_HOLDER.length(), offset, pos, andOrBit, level );
        }
        else
        {
            output->add( text, len, offset, pos, andOrBit, level );
        }
    }

    virtual int analyze_impl( const Term& input, void* data, HookType func ) = 0;

protected:

    Tokenizer tokenizer_;

    /// For a secondary analysis based on analyzed output (e.g. synonym output), if needed.
    boost::shared_ptr<la::Analyzer> innerAnalyzer_;

    /// Whether including speical characters (e.g. puncutations) in the result.
    bool bExtractSpecialChar_;

    /// Whether converting speical characters (e.g. puncutations) into a particular place holder
    /// symbol in the result.
    /// Be effect only when bExtractSpecialChar_ is set.
    bool bConvertToPlaceHolder_;

};
}

#endif /* _LA_ANALYZER_H_ */
