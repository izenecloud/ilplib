#ifndef _JAPANESE_ANALYZER_H_
#define _JAPANESE_ANALYZER_H_

#include <la/analyzer/CommonLanguageAnalyzer.h>

#include <ijma.h>
#include <iostream>
namespace la
{

class JapaneseAnalyzer : public CommonLanguageAnalyzer
{
public:

    JapaneseAnalyzer( const std::string knowledgePath);

    ~JapaneseAnalyzer();

    void setIndexMode();

    void setLabelMode();

    inline  void setNBest( unsigned int num=2 )
    {
        pA_->setOption( jma::Analyzer::OPTION_TYPE_NBEST, num );
    }

protected:

    inline void parse(const UString & input)
    {
        // Since cma accept a whole article as input,
        // it maybe extraordinary long sometimes.
        if(input_string_buffer_size_ < input.length()*3+1)
        {
            while(input_string_buffer_size_ < input.length()*3+1)
            {
                input_string_buffer_size_ *= 2;
            }
            delete input_string_buffer_;
            input_string_buffer_ = new char[input_string_buffer_size_];
        }

        input.convertString(izenelib::util::UString::UTF_8,
                            input_string_buffer_, input_string_buffer_size_);
        pS_->setString( input_string_buffer_ );
        pA_->runWithSentence( *pS_ );

        listIndex_ = 0;
        lexiconIndex_ = -1;
        localOffset_ = -1;

        resetToken();
    }

    inline bool nextToken()
    {
        while(doNext())
        {
            nativeToken_ = pS_->getLexicon(listIndex_, lexiconIndex_);
            nativeTokenLen_ = strlen(nativeToken_);

            if(nativeTokenLen_ > term_ustring_buffer_limit_ )
                continue;

            token_ = output_ustring_buffer_;
            len_ = UString::toUcs2(izenelib::util::UString::UTF_8,
                                   nativeToken_, nativeTokenLen_, output_ustring_buffer_,
                                   term_ustring_buffer_limit_);

            morpheme_ = pS_->getPOS(listIndex_, lexiconIndex_);
            pos_ = pS_->getStrPOS(listIndex_, lexiconIndex_);
            isIndex_ = true;//pS_->isIndexWord(listIndex_, lexiconIndex_);
            offset_ = localOffset_;
            return true;
        }
        resetToken();
        return false;
    }

    inline bool isAlpha()
    {
        return morpheme_ == flMorp_;
    }

    inline bool isSpecialChar()
    {
        return morpheme_ == scMorp_;
    }

private:

    inline bool doNext()
    {
        if(listIndex_ == pS_->getListSize())
        {
            return false;
        }
        ++ lexiconIndex_;

        if( lexiconIndex_ == pS_->getCount(listIndex_) )
        {
            do {
                ++ listIndex_;
                lexiconIndex_ = 0;
                localOffset_ = 0;
                if(listIndex_ == pS_->getListSize()) return false;
            } while( lexiconIndex_ == pS_->getCount(listIndex_) );
        }
        else
        {
            ++ localOffset_;
        }

        return true;
    }

private:

    jma::Analyzer * pA_;

    jma::Sentence * pS_;

    size_t input_string_buffer_size_;
    char * input_string_buffer_;

    UString::CharT * output_ustring_buffer_;

    int listIndex_;

    int lexiconIndex_;

    unsigned int morpheme_;

    unsigned int flMorp_;

    unsigned int scMorp_;

    string segment_;
};

}

#endif
