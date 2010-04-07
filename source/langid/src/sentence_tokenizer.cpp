/** \file sentence_tokenizer.cpp
 * Implementation of class SentenceTokenizer.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Dec 04, 2009
 */

#include "sentence_tokenizer.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cstring>

using namespace std;

#ifdef LANGID_DEBUG_PRINT
    #undef LANGID_DEBUG_PRINT
	#define LANGID_DEBUG_PRINT 0
#endif

namespace
{
/** bit mask of paragraph separators */
const int MASK_SEP = langid::SB_TYPE_SEP | langid::SB_TYPE_CR | langid::SB_TYPE_LF;

/** bit mask of sentence terminators */
const int MASK_TERM = langid::SB_TYPE_STERM | langid::SB_TYPE_ATERM;

/** bit mask of sentence separators, terminators and continues */
const int MASK_SEP_TERM_CONTINUE = MASK_SEP | MASK_TERM | langid::SB_TYPE_SCONTINUE;

/** bit mask of negation of letters, sentence separators and terminators */
const int MASK_NEG_LETTER = ~(langid::SB_TYPE_OLETTER | langid::SB_TYPE_UPPER | langid::SB_TYPE_LOWER | MASK_SEP | MASK_TERM);
} // namespace

namespace langid
{

SentenceTokenizer::SentenceTokenizer(const SentenceBreakTable& table)
    : breakTable_(table)
{
}

int SentenceTokenizer::getSentenceLength(const char* str) const
{
#if LANGID_DEBUG_PRINT
    cout << endl << ">>> SentenceTokenizer::getSentenceLength(), str:" << endl;
    cout << str << endl;
#endif

    assert(str);

    if(*str == 0)
    {
#if LANGID_DEBUG_PRINT
        cout << "<<< SentenceTokenizer::getSentenceLength(), end of string." << endl;
#endif
        return 0;
    }

    unsigned short ucs;
    const char* p = str;
    const char* q;
    const char* end = str + strlen(str);
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
        ucs = encoding_.convertToUCS2(q, end, &m[j]);
        t[j] = breakTable_.getProperty(ucs);
        assert(q < end || (m[j] == 0 && t[j] == SB_TYPE_OTHER));
#if LANGID_DEBUG_PRINT
        cout << "new c[" << j << "]: ";
        debugPrintUCS(q, m[j], ucs, t[j]);
#endif
        q += m[j];
    }

    while(*p && ! isEnd)
    {
#if LANGID_DEBUG_PRINT
        cout << "iteration : " << dec << iter++ << ", char index: " << (p - str) << endl;
        q = p;
        for(int j=0; j<3; ++j)
        {
            cout << "c[" << j << "]: ";
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
            cout << "using rule 3.0" << endl;
#endif
        }
        // rule 4.0
        else if(t[0] & MASK_SEP)
        {
            count = 1;
            isEnd = true;
#if LANGID_DEBUG_PRINT
            cout << "using rule 4.0" << endl;
#endif
        }
        // rule 6.0
        else if(t[0] == SB_TYPE_ATERM && t[1] == SB_TYPE_NUMERIC)
        {
            count = 1;
#if LANGID_DEBUG_PRINT
            cout << "using rule 6.0" << endl;
#endif
        }
        // rule 7.0
        else if(t[0] == SB_TYPE_UPPER && t[1] == SB_TYPE_ATERM && t[2] == SB_TYPE_UPPER)
        {
            count = 2;
#if LANGID_DEBUG_PRINT
            cout << "using rule 7.0" << endl;
#endif
        }
        else if(t[0] & MASK_TERM)
        {
#if LANGID_DEBUG_PRINT
            cout << "c[1] SB_TYPE_CLOSE*: ";
#endif
            q = p + m[0];
            m[1] = starMatch(q, end, SB_TYPE_CLOSE);

#if LANGID_DEBUG_PRINT
            cout << "c[2] SB_TYPE_SP*: ";
#endif
            q += m[1];
            m[2] = starMatch(q, end, SB_TYPE_SP);

#if LANGID_DEBUG_PRINT
            cout << "c[3] MASK_NEG_LETTER*: ";
#endif
            q += m[2];
            m[3] = starMatch(q, end, MASK_NEG_LETTER);

            ucs = encoding_.convertToUCS2(q + m[3], end, &m[4]);
            t[4] = breakTable_.getProperty(ucs);
            assert(q + m[3] < end || (m[4] == 0 && t[4] == SB_TYPE_OTHER));
#if LANGID_DEBUG_PRINT
            cout << "c[4]: ";
            debugPrintUCS(q + m[3], m[4], ucs, t[4]);
#endif

            // rule 8.0
            if(t[0] == SB_TYPE_ATERM && t[4] == SB_TYPE_LOWER)
            {
                count = 4;
#if LANGID_DEBUG_PRINT
                cout << "using rule 8.0" << endl;
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
                    ucs = encoding_.convertToUCS2(q, end, &m[3]);
                    t[3] = breakTable_.getProperty(ucs);
                    assert(q < end || (m[3] == 0 && t[3] == SB_TYPE_OTHER));
                }
#if LANGID_DEBUG_PRINT
                cout << "c[3]: ";
                debugPrintUCS(q, m[3], ucs, t[3]);
#endif

                // rule 8.1 & 11.0
                if((t[3] & MASK_SEP_TERM_CONTINUE) == 0)
                {
                    isEnd = true;
#if LANGID_DEBUG_PRINT
                    cout << "using rule 11.0" << endl;
#endif
                }
#if LANGID_DEBUG_PRINT
                else if(t[3] & MASK_SEP)
                    cout << "using rule 11.0" << endl;
                else
                    cout << "using rule 8.1" << endl;
#endif
            }
        }
        // rule 12.0
        else
        {
            count = 1;
#if LANGID_DEBUG_PRINT
            cout << "using rule 12.0" << endl;
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
            ucs = encoding_.convertToUCS2(q, end, &m[j]);
            t[j] = breakTable_.getProperty(ucs);
            assert(q < end || (m[j] == 0 && t[j] == SB_TYPE_OTHER));
#if LANGID_DEBUG_PRINT
            cout << "new c[" << j << "]: ";
            debugPrintUCS(q, m[j], ucs, t[j]);
#endif
            q += m[j];
        }

#if LANGID_DEBUG_PRINT
        cout << "virtual characters count: " << count << ", isEnd: " << isEnd << endl;
        cout << endl;
#endif
    }

    int len = p - str;

#if LANGID_DEBUG_PRINT
    cout << "<<< SentenceTokenizer::getSentenceLength(), sentence: " << string(str, len)  << ", length: " << len << endl;
#endif

    return len;
}

std::size_t SentenceTokenizer::starMatch(const char* begin, const char* end, int mask) const
{
    assert(begin <= end && "range is invalid in SentenceTokenizer::starMatch()");

    unsigned short ucs;
    const char* p = begin;
    size_t mblen;
    SentenceBreakType type;

    while(p < end)
    {
        ucs = encoding_.convertToUCS2(p, end, &mblen);
        type = breakTable_.getProperty(ucs);

        if((type & mask) == 0)
            break;
#if LANGID_DEBUG_PRINT
        else
        {
            string propStr;
            if(breakTable_.propertyToStr(type, propStr))
                //cout << "[" << string(p, mblen) << "_0x" << setw(4) << setfill('0') << hex << ucs << "_" << propStr << "] ";
                cout << "[" << string(p, mblen) << "_0x" << setw(4) << setfill('0') << hex << ucs << "_" << propStr << "_" << type << "] ";
            else
                cerr << "error: unknown property: " << type << endl;
        }
#endif

        p += mblen;
    }

#if LANGID_DEBUG_PRINT
    cout << "mask: " << hex << mask << ", match bytes: " << (p - begin) << endl;
#endif
    return p - begin;
}

void SentenceTokenizer::debugPrintUCS(const char* s, std::size_t n, unsigned short ucs, SentenceBreakType type) const
{
    string propStr;
    if(breakTable_.propertyToStr(type, propStr))
        cout << "[" << string(s, n) << "_0x" << setw(4) << setfill('0') << hex << ucs << "_" << propStr << "]" << endl;
    else
        cerr << "error: unknown property: " << type << endl;
}

void SentenceTokenizer::debugPrintUCS(const char* s, std::size_t n, SentenceBreakType type) const
{
    string propStr;
    if(breakTable_.propertyToStr(type, propStr))
        cout << "[" << string(s, n) << "_" << propStr << "]" << endl;
    else
        cerr << "error: unknown property: " << type << endl;
}

} // namespace langid
