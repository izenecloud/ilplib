/**
 * @author Wei
 */

#ifndef _KOREAN_ANALYZER_H_
#define _KOREAN_ANALYZER_H_

#include <la/analyzer/CommonLanguageAnalyzer.h>

#include <wk_eojul.h>
#include <wk_analyzer.h>
#include <wk_pos.h>

namespace la
{

class KoreanAnalyzer : public CommonLanguageAnalyzer
{

public:

    KoreanAnalyzer( const std::string knowledgePath, bool loadModel = true );

    ~KoreanAnalyzer();

    virtual void setIndexMode();

    virtual void setLabelMode();

    virtual void setNBest( unsigned int num=2 )
    {
        pA_->setOption(kmaOrange::WKO_OPTION_N_BEST, num);
    }

    virtual void setCaseSensitive(bool casesensitive = true, bool containlower = true)
    {
        int val = casesensitive ? 1 : 0;
        pA_->setOption( kmaOrange::WKO_OPTION_CASE_SENSITIVE, val );
        CommonLanguageAnalyzer::setCaseSensitive(casesensitive, containlower);
    }

    /**
     * @brief  Sets the lower bound digit limit of the numbers extracted
     */
    inline void setLowDigitBound( unsigned int num=1 )
    {
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_NUM, num );
    }

    /**
     * @brief   Whether to combine number dependent noun to extracted term
     */
    inline void setCombineBoundNoun( bool flag=false )
    {
        pA_->setOption(kmaOrange::WKO_OPTION_COMBINE_BOUND_NOUN, (flag) ? 1 : 0);
    }

    /**
     * @brief   Whether or not to extract stem terms of verb and adj words
     */
    inline void setVerbAdjStems( bool flag=false )
    {
        pA_->setOption(kmaOrange::WKO_OPTION_EXTRACT_VERB_STEMS, (flag) ? 1 : 0);
    }

protected:

    void parse(const UString & input)
    {
        if(input_string_buffer_size_ < input.length()*3+1)
        {
            while(input_string_buffer_size_ < input.length()*3+1)
            {
                input_string_buffer_size_ *= 2;
            }
            delete input_string_buffer_;
            input_string_buffer_ = new char[input_string_buffer_size_];
        }
        input.convertString(izenelib::util::UString::CP949,
                            input_string_buffer_, input_string_buffer_size_);
//        pS_->initialize();
//        pS_->setString( sentence );
//        pA_->runWithEojul();
        pA_->runWithString(input_string_buffer_);

        eojul_ = NULL;
        eojulIndex_ = 0;
        listIndex_ = 0;
        lexiconIndex_ = -1;

        resetToken();
    }

    bool nextToken()
    {
//        if(listIndex_ == pS_->getListSize()) {
//            resetToken();
//            return false;
//        }
//
//        token_ = pS_->getLexicon(listIndex_, lexiconIndex_);
//        len_ = strlen(token_);
//        morpheme_ = pS_->getPOS(listIndex_, lexiconIndex_);
//        offset_ = sentenceOffset_;
//        needIndex_ = pS_->isIndexWord(listIndex_, lexiconIndex_);
//
//        ++ lexiconIndex_;
//
//        while(lexiconIndex_ == pS_->getCount(listIndex_)) {
//            ++ listIndex_;
//            lexiconIndex_ = 0;
//            if(listIndex_ == pS_->getListSize()) break;
//        }
//
//        return true;

        while(doNext())
        {
            if(!eojul_)
            {
                eojul_ = pA_->getEojulInSentence(eojulIndex_);

                nativeToken_ = eojul_->getString();
                nativeTokenLen_ = strlen(nativeToken_);
                if(nativeTokenLen_>term_ustring_buffer_limit_)
                {
                    continue;
                }

                token_ = output_ustring_buffer_;
                len_ = UString::toUcs2(izenelib::util::UString::CP949,
                                       nativeToken_, nativeTokenLen_, output_ustring_buffer_,
                                       term_ustring_buffer_limit_);

                if(eojul_->getListSize()==1 && eojul_->getCount(0)==1)
                {
                    morpheme_ = eojul_->getPOS(0, 0);
                    pos_ = eojul_->getStrPOS(0,0);
                }
                else
                {
                    morpheme_ = 0;
                    pos_ = NULL;
                }
                offset_ = eojulIndex_;
                level_ = 0;
                isIndex_ = true;
                isRaw_ = true;
                return true;
            }
            else
            {
                nativeToken_ = eojul_->getLexicon(listIndex_, lexiconIndex_);
                nativeTokenLen_ = strlen(nativeToken_);
                if(nativeTokenLen_>term_ustring_buffer_limit_)
                {
                    continue;
                }

                morpheme_ = eojul_->getPOS(listIndex_, lexiconIndex_);
                pos_ = eojul_->getStrPOS(listIndex_, lexiconIndex_);
                token_ = output_ustring_buffer_;
                len_ = UString::toUcs2(izenelib::util::UString::CP949,
                                       nativeToken_, nativeTokenLen_, output_ustring_buffer_,
                                       term_ustring_buffer_limit_);

                level_ = 1;
                //isIndex_ = eojul_->isIndexWord(listIndex_, lexiconIndex_);
                isIndex_ = ((morpheme_&kmaOrange::N_) ||
                            (morpheme_==kmaOrange::FL) ||
                            (morpheme_==kmaOrange::SN) ||
                            (morpheme_==kmaOrange::SC) );
                isRaw_ = false;
                return true;
            }
        }
        resetToken();
        return false;
    }

    inline bool isAlpha()
    {
        return morpheme_ == kmaOrange::FL;
    }

    inline bool isSpecialChar()
    {
        return morpheme_ == kmaOrange::SC;
    }

private:


    bool doNext()
    {
        if(eojulIndex_ == pA_->getEojulCountInSentence() )
        {
            return false;
        }
        if(eojul_)
        {
            ++ lexiconIndex_;
            if(lexiconIndex_ == eojul_->getCount(listIndex_))
            {
                lexiconIndex_ = 0;
                ++ listIndex_;
            }
            // Check the new dividuation of the eojul to see whether we need to skip it.
            while(listIndex_ < eojul_->getListSize() && eojul_->getCount(listIndex_) == 1 )
            {
                ++ listIndex_;
            }
            if(listIndex_ == eojul_->getListSize())
            {
                lexiconIndex_ = -1;
                listIndex_ = 0;
                ++ eojulIndex_;
                eojul_ = NULL;
                if(eojulIndex_ == pA_->getEojulCountInSentence() )
                {
                    return false;
                }
            }
        }
        return true;
    }

private:

    kmaOrange::WK_Analyzer * pA_;

//    kmaOrange::WK_Eojul * pS_;

    size_t input_string_buffer_size_;
    char * input_string_buffer_;

    UString::CharT * output_ustring_buffer_;

    kmaOrange::WK_Eojul * eojul_;

    int eojulIndex_;

    int listIndex_;

    int lexiconIndex_;

    unsigned int morpheme_;

};
}

#endif
