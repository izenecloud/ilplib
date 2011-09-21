/** \file sentence_tokenizer.cpp
 * Implementation of class SentenceTokenizer.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 04, 2009
 */

#include "ucs2_converter.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cstring>

#define LANGID_DEBUG_PRINT 0

NS_ILPLIB_LANGID_BEGIN

template<EncodingID encoding>
std::size_t SentenceTokenizer::getSentenceLength(const char* begin, const char* end) const
{
#if LANGID_DEBUG_PRINT
    std::std::cout << std::std::endl << ">>> SentenceTokenizer::getSentenceLength(), begin:" << std::std::endl;
    std::std::cout << begin << std::std::endl;
#endif

    if(!begin || !end || begin >= end)
    {
#if LANGID_DEBUG_PRINT
        std::std::cout << "<<< SentenceTokenizer::getSentenceLength(), end of string." << std::std::endl;
#endif
        return 0;
    }

    typedef UCS2_Converter<encoding> UCSConv;
    unsigned short ucs;
    const char* p = begin;
    const char* q;
    bool isEnd = false;

    int count; // count of characters skipped in each iteration
    size_t m[5]; // bytes count
    SentenceBreakType t[5]; // character type

#if LANGID_DEBUG_PRINT
    int iter = 0;
#endif

    // init array m[0..2], t[0..2]
    q = p;
    for(int j=0; j<3; ++j)
    {
        ucs = UCSConv::convertToUCS2(q, end, &m[j]);
        t[j] = breakTable_.getProperty(ucs);
        assert(q < end || (m[j] == 0 && t[j] == SB_TYPE_OTHER));
#if LANGID_DEBUG_PRINT
        std::cout << "new c[" << j << "]: ";
        debugPrintUCS(q, m[j], ucs, t[j]);
#endif
        q += m[j];
    }

    while(p < end && ! isEnd)
    {
#if LANGID_DEBUG_PRINT
        std::cout << "iteration : " << dec << iter++ << ", char index: " << (p - begin) << std::endl;
        q = p;
        for(int j=0; j<3; ++j)
        {
            std::cout << "c[" << j << "]: ";
            debugPrintUCS(q, m[j], t[j]);
            q += m[j];
        }
#endif

        count = 0;

        // rule 3.0
        if(t[0] == SB_TYPE_CR && t[1] == SB_TYPE_LF)
        {
            count = 1;
#if LANGID_DEBUG_PRINT
            std::cout << "using rule 3.0" << std::endl;
#endif
        }
        // rule 4.0
        else if(t[0] & MASK_SEP)
        {
            count = 1;
            isEnd = true;
#if LANGID_DEBUG_PRINT
            std::cout << "using rule 4.0" << std::endl;
#endif
        }
        // rule 6.0
        else if(t[0] == SB_TYPE_ATERM && t[1] == SB_TYPE_NUMERIC)
        {
            count = 1;
#if LANGID_DEBUG_PRINT
            std::cout << "using rule 6.0" << std::endl;
#endif
        }
        // rule 7.0
        else if(t[0] == SB_TYPE_UPPER && t[1] == SB_TYPE_ATERM && t[2] == SB_TYPE_UPPER)
        {
            count = 2;
#if LANGID_DEBUG_PRINT
            std::cout << "using rule 7.0" << std::endl;
#endif
        }
        else if(t[0] & MASK_TERM)
        {
#if LANGID_DEBUG_PRINT
            std::cout << "c[1] SB_TYPE_CLOSE*: ";
#endif
            q = p + m[0];
            m[1] = starMatch<encoding>(q, end, SB_TYPE_CLOSE);

#if LANGID_DEBUG_PRINT
            std::cout << "c[2] SB_TYPE_SP*: ";
#endif
            q += m[1];
            m[2] = starMatch<encoding>(q, end, SB_TYPE_SP);

#if LANGID_DEBUG_PRINT
            std::cout << "c[3] MASK_NEG_LETTER*: ";
#endif
            q += m[2];
            m[3] = starMatch<encoding>(q, end, MASK_NEG_LETTER);

            ucs = UCSConv::convertToUCS2(q + m[3], end, &m[4]);
            t[4] = breakTable_.getProperty(ucs);
            assert(q + m[3] < end || (m[4] == 0 && t[4] == SB_TYPE_OTHER));
#if LANGID_DEBUG_PRINT
            std::cout << "c[4]: ";
            debugPrintUCS(q + m[3], m[4], ucs, t[4]);
#endif

            // rule 8.0
            if(t[0] == SB_TYPE_ATERM && t[4] == SB_TYPE_LOWER)
            {
                count = 4;
#if LANGID_DEBUG_PRINT
                std::cout << "using rule 8.0" << std::endl;
#endif
            }
            else
            {
                count = 3;

                // replace m[3] with m[4]
                if(m[3] == 0)
                {
                    m[3] = m[4];
                    t[3] = t[4];
                }
                // recalculate m[3]
                else
                {
                    assert(q == p + m[0] + m[1] + m[2]);
                    ucs = UCSConv::convertToUCS2(q, end, &m[3]);
                    t[3] = breakTable_.getProperty(ucs);
                    assert(q < end || (m[3] == 0 && t[3] == SB_TYPE_OTHER));
                }
#if LANGID_DEBUG_PRINT
                std::cout << "c[3]: ";
                debugPrintUCS(q, m[3], ucs, t[3]);
#endif

                // rule 8.1 & 11.0
                if((t[3] & MASK_SEP_TERM_CONTINUE) == 0)
                {
                    isEnd = true;
#if LANGID_DEBUG_PRINT
                    std::cout << "using rule 11.0" << std::endl;
#endif
                }
#if LANGID_DEBUG_PRINT
                else if(t[3] & MASK_SEP)
                    std::cout << "using rule 11.0" << std::endl;
                else
                    std::cout << "using rule 8.1" << std::endl;
#endif
            }
        }
        // rule 12.0
        else
        {
            count = 1;
#if LANGID_DEBUG_PRINT
            std::cout << "using rule 12.0" << std::endl;
#endif
        }

        assert(count > 0 && count < 5 && "count of characters skipped should be in range [1, 4].");
        // advance p
        for(int i=0; i<count; ++i)
        {
            p += m[i];
        }
        // update array m[0..2], t[0..2]
        int j=0;
        for(q=p; j<3-count; ++j)
        {
            m[j] = m[j+count];
            t[j] = t[j+count];
            q += m[j];
        }
        for(; j<3; ++j)
        {
            ucs = UCSConv::convertToUCS2(q, end, &m[j]);
            t[j] = breakTable_.getProperty(ucs);
            assert(q < end || (m[j] == 0 && t[j] == SB_TYPE_OTHER));
#if LANGID_DEBUG_PRINT
            std::cout << "new c[" << j << "]: ";
            debugPrintUCS(q, m[j], ucs, t[j]);
#endif
            q += m[j];
        }

#if LANGID_DEBUG_PRINT
        std::cout << "virtual characters count: " << count << ", isEnd: " << isEnd << std::endl;
        std::cout << std::endl;
#endif
    }

    size_t len = p - begin;

#if LANGID_DEBUG_PRINT
    std::cout << "<<< SentenceTokenizer::getSentenceLength(), sentence: " << std::string(begin, len)  << ", length: " << len << std::endl;
#endif

    return len;
}

template<EncodingID encoding>
std::size_t SentenceTokenizer::starMatch(const char* begin, const char* end, int mask) const
{
    assert(begin <= end && "range is invalid in SentenceTokenizer::starMatch()");

    typedef UCS2_Converter<encoding> UCSConv;
    unsigned short ucs;
    const char* p = begin;
    size_t mblen;
    SentenceBreakType type;

    while(p < end)
    {
        ucs = UCSConv::convertToUCS2(p, end, &mblen);
        type = breakTable_.getProperty(ucs);

        if((type & mask) == 0)
            break;
#if LANGID_DEBUG_PRINT
        else
        {
            std::string propStr;
            if(breakTable_.propertyToStr(type, propStr))
                //std::cout << "[" << std::string(p, mblen) << "_0x" << setw(4) << setfill('0') << hex << ucs << "_" << propStr << "] ";
                std::cout << "[" << std::string(p, mblen) << "_0x" << setw(4) << setfill('0') << hex << ucs << "_" << propStr << "_" << type << "] ";
            else
                std::cerr << "error: unknown property: " << type << std::endl;
        }
#endif

        p += mblen;
    }

#if LANGID_DEBUG_PRINT
    std::cout << "mask: " << hex << mask << ", match bytes: " << (p - begin) << std::endl;
#endif
    return p - begin;
}

inline void SentenceTokenizer::debugPrintUCS(const char* s, std::size_t n, unsigned short ucs, SentenceBreakType type) const
{
    std::string propStr;
    if(breakTable_.propertyToStr(type, propStr))
        std::cout << "[" << std::string(s, n) << "_0x" << std::setw(4) << std::setfill('0') << std::hex << ucs << "_" << propStr << "]" << std::endl;
    else
        std::cerr << "error: unknown property: " << type << std::endl;
}

inline void SentenceTokenizer::debugPrintUCS(const char* s, std::size_t n, SentenceBreakType type) const
{
    std::string propStr;
    if(breakTable_.propertyToStr(type, propStr))
        std::cout << "[" << std::string(s, n) << "_" << propStr << "]" << std::endl;
    else
        std::cerr << "error: unknown property: " << type << std::endl;
}

NS_ILPLIB_LANGID_END
