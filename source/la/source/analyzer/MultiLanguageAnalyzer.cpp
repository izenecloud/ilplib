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
using namespace ilplib::langid;

namespace la
{

MultiLanguageAnalyzer::MultiLanguageAnalyzer() : Analyzer() {}

MultiLanguageAnalyzer::~MultiLanguageAnalyzer() {}

void MultiLanguageAnalyzer::setAnalyzer(Language lang, boost::shared_ptr<Analyzer>& analyzer)
{
    if (lang == OTHER)
        return;
    analyzers_[ lang ] = analyzer;
}

boost::shared_ptr<Analyzer> MultiLanguageAnalyzer::getAnalyzer(MultiLanguageAnalyzer::Language lang) const
{
    if (lang == OTHER)
        return boost::shared_ptr<Analyzer>();
    return analyzers_[ lang ];
}


void MultiLanguageAnalyzer::setDefaultAnalyzer(boost::shared_ptr<Analyzer>& defAnalyzer)
{
    defAnalyzer_ = defAnalyzer;
}

boost::shared_ptr<Analyzer> MultiLanguageAnalyzer::getDefaultAnalyzer() const
{
    return defAnalyzer_;
}

MultiLanguageAnalyzer::Language MultiLanguageAnalyzer::getCharType(UCS2Char ucs2Char)
{
    if (UString::isThisChineseChar(ucs2Char))
        return CHINESE;

    if (UString::isThisAlphaChar(ucs2Char))
        return ENGLISH;

    if (UString::isThisKoreanChar(ucs2Char))
        return KOREAN;

    if (UString::isThisJapaneseChar(ucs2Char))
        return JAPANESE;

    return OTHER;
}

MultiLanguageAnalyzer::Language MultiLanguageAnalyzer::detectLanguage(const UString & input)
{
    LanguageID langId = LANGUAGE_ID_UNKNOWN;

    langIdAnalyzer_->languageFromString(input, langId);
    switch (langId)
    {
    case LANGUAGE_ID_CHINESE_SIMPLIFIED:
    case LANGUAGE_ID_CHINESE_TRADITIONAL:
        return CHINESE;
    case LANGUAGE_ID_JAPANESE:
        return JAPANESE;
    case LANGUAGE_ID_KOREAN:
        return KOREAN;
    case LANGUAGE_ID_ENGLISH:
        return ENGLISH;
    default:
        return OTHER;
    }
}

/// obsolete
void MultiLanguageAnalyzer::analyzeSynonym(TermList& output, size_t n)
{
    if (analyzers_[CHINESE])
        analyzers_[CHINESE]->analyzeSynonym(output, n);
}

int MultiLanguageAnalyzer::analyzeSynonym(const izenelib::util::UString& inputString, TermList& output)
{
    Language lang = detectLanguage(inputString);

    if (lang != OTHER && analyzers_[lang])
    {
        return analyzers_[lang]->analyzeSynonym(inputString, output);
    }
    else if (defAnalyzer_)
    {
        return defAnalyzer_->analyzeSynonym(inputString, output);
    }
    else
        return 0;
}

ilplib::langid::Analyzer* MultiLanguageAnalyzer::langIdAnalyzer_;

int MultiLanguageAnalyzer::analyze_impl(const Term& input, void* data, HookType func)
{
    if (!langIdAnalyzer_)
    {
        if (defAnalyzer_)
        {
            return defAnalyzer_->analyze_impl(input, data, func);
        }
        else
        {
            return 0;
        }
    }

    Language lang = detectLanguage(input.text_);
    if (lang != OTHER && analyzers_[lang])
    {
        return analyzers_[lang]->analyze_impl(input, data, func);
    }
    else if (defAnalyzer_)
    {
        return defAnalyzer_->analyze_impl(input, data, func);
    }
    else
    {
        return 0;
    }
}

int MultiLanguageAnalyzer::analyze_impl(const Term& input, void* data, HookType func, MultilangGranularity multilangGranularity)
{
    if (!langIdAnalyzer_ || multilangGranularity != SENTENCE_LEVEL)
        return analyze_impl(input, data, func);

    void** parameters = (void**)data;
    TermList * output = (TermList*) parameters[0];
    const izenelib::util::UString& text = input.text_;
    std::size_t textPos = 0;
    Term sentence;
    UString& stext = sentence.text_;

    std::size_t lastpos=0, pos=0, globalOffset=0;
    while (std::size_t len = langIdAnalyzer_->sentenceLength(text, textPos))
    {
        stext.assign(text, textPos, len);
        Language lang = detectLanguage(stext);

        std::size_t localOffset;
        if (lang != OTHER && analyzers_[lang])
            localOffset = analyzers_[lang]->analyze_impl(sentence, data, func);
        else if (defAnalyzer_)
            localOffset = defAnalyzer_->analyze_impl(sentence, data, func);
        else
            return 0;

        lastpos = pos;
        pos = output->size();

        if (lastpos > 0)
        {
            TermList& laInput = (*output);
            for(std::size_t i = lastpos; i < pos; ++i)
                laInput[i].wordOffset_ += globalOffset;
        }
        textPos += len;
        globalOffset += localOffset;
    }

    return globalOffset;
}
}
