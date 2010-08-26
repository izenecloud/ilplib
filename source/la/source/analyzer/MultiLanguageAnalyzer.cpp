/**
 * @brief Analyzer for multi-languages
 * @file MultiLanguageAnalyzer.h
 *
 * @date Jan 4, 2010
 * @author vernkin
 */

/**
 * @brief Rewrite MLA using a fast language detction,
 *     String mode is moved to EnglishAnalyzer,
 *      Char mode is moved to CharAnalyzer.
 * @author Wei
 */


#include "la/analyzer/MultiLanguageAnalyzer.h"

#include <iostream>
using namespace std;
using namespace izenelib::util;

namespace la
{

MultiLanguageAnalyzer::MultiLanguageAnalyzer() : Analyzer() {}

MultiLanguageAnalyzer::~MultiLanguageAnalyzer() {}

void MultiLanguageAnalyzer::setAnalyzer( Language lang, shared_ptr<Analyzer>& analyzer )
{
    if( lang == OTHER )
        return;
    analyzers_[ lang ] = analyzer;
}

shared_ptr<Analyzer> MultiLanguageAnalyzer::getAnalyzer( MultiLanguageAnalyzer::Language lang ) const
{
    if( lang == OTHER )
        return shared_ptr<Analyzer>();
    return analyzers_[ lang ];
}


void MultiLanguageAnalyzer::setDefaultAnalyzer( shared_ptr<Analyzer>& defAnalyzer )
{
    defAnalyzer_ = defAnalyzer;
}

shared_ptr<Analyzer> MultiLanguageAnalyzer::getDefaultAnalyzer() const
{
    return defAnalyzer_;
}

MultiLanguageAnalyzer::Language MultiLanguageAnalyzer::getCharType( UCS2Char ucs2Char )
{
    if(UString::isThisChineseChar( ucs2Char ) )
        return CHINESE;

    if(UString::isThisAlphaChar( ucs2Char ) )
        return ENGLISH;

    if(UString::isThisKoreanChar( ucs2Char ) )
        return KOREAN;

    if(UString::isThisJapaneseChar( ucs2Char ) )
        return JAPANESE;

    return OTHER;
}

MultiLanguageAnalyzer::Language MultiLanguageAnalyzer::detectLanguage( const UString & input )
{
    static const size_t SampleCount = 10;
    if(input.length() < SampleCount)
    {
        for(size_t i = 0; i< input.length(); i++ )
        {
            if(UString::isThisLanguageChar(input.at(i)))
            {
                Language lang = getCharType(input.at(i));
                if(lang == CHINESE) return CHINESE;
                if(lang == KOREAN) return KOREAN;
                if(lang == JAPANESE) return JAPANESE;
                //if(lang == OTHER) return OTHER;
            }
        }
        return ENGLISH;
    }

    for( size_t i = 0; i < SampleCount; i++ )
    {
        size_t index = i*input.length()/SampleCount;
        while(index<input.length()-1 && !UString::isThisLanguageChar(input.at(index)))
            index ++;
        if(UString::isThisLanguageChar(input.at(index)))
        {
            Language lang = getCharType(input.at(index));
            if(lang == CHINESE) return CHINESE;
            if(lang == KOREAN) return KOREAN;
            if(lang == JAPANESE) return JAPANESE;
            //if(lang == OTHER) return OTHER;
        }
    }
    return ENGLISH;
}

int MultiLanguageAnalyzer::analyze_impl( const Term& input, void* data, HookType func )
{
    Language lang = detectLanguage(input.text_);
    if(lang != OTHER && analyzers_[lang])
    {
        return analyzers_[lang]->analyze_impl(input, data, func);
    }
    else if(defAnalyzer_)
    {
        return defAnalyzer_->analyze_impl(input, data, func);
    }
    else
    {
        return 0;
    }
}

}
