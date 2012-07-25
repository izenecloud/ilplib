/*
 * LA.cpp
 *
 *  Created on: 2009-6-16
 *      Author: zhjay
 */

#include <la/LA.h>
#include <la/util/EnglishUtil.h>

#include <util/ustring/UString.h>

#include <map>

using namespace la::stem;
using namespace izenelib::util;
using namespace izenelib::ir::idmanager;
using namespace std;

namespace la
{

    const string RES_PUNCT = "RP";
    const Term newTerm;

    const UString OP_USTR("&|!(){}[]^\"", UString::UTF_8);
    const UString BACK_SLASH("\\", UString::UTF_8);

    map< UString, UString > OP_REP_MAP;

    void replaceSpecialChar(const UString& in, UString& ret)
    {
        map< UString, UString >::iterator itr;
        for (size_t i = 0; i < in.length(); ++i)
        {
            UString key = in.substr(i, 1);
            itr = OP_REP_MAP.find(key);
            if (itr == OP_REP_MAP.end())
            {
                ret += key;
            }
            else
            {
                ret += itr->second;
            }
        }
    }

    LA::LA()
    {
        if (OP_REP_MAP.empty())
        {
            for (size_t i = 0; i < OP_USTR.length(); ++i)
            {
                UString key = OP_USTR.substr(i, 1);
                OP_REP_MAP[ key ] = BACK_SLASH;
                OP_REP_MAP[ key ] += key;
            }
        }
    }

    /// Adjust terms' offest or level, for the following expansions:
    /// 1. abc123,abc,123 -> "(abc123|(abc&123))"
    ///
    void preProcess(const TermList & ktermList)
    {
        TermList& termList = const_cast<TermList&>(ktermList);

        TermList::iterator it = termList.begin();
        while (it != termList.end())
        {
            if (it->text_.length() <= 0)
            {
                it++;
                continue;
            }

            UString::CharT& ch =  it->text_[0];
            //cout<<endl; it->text_.displayStringValue(izenelib::util::UString::UTF_8);
            if (UString::isThisDigitChar(ch) || UString::isThisAlphaChar(ch))
            {
                // e.g abc123
                unsigned int curOffset = it->wordOffset_;
                unsigned int curLevel = it->getLevel();
                UString& curText = it->text_;
                size_t start = 0;
                size_t end = curText.length();

                if (++it == termList.end() || it->wordOffset_ != curOffset)
                    continue;

                // e.g abc, 123
                while (it != termList.end())
                {
                    //it->text_.displayStringValue(izenelib::util::UString::UTF_8); cout<<endl;
                    bool match = true;
                    for (size_t i = 0; i < it->text_.length() && start < end; i++)
                    {
                        if (it->text_[i] == curText[start++])
                            continue;

                        match = false;
                        break;
                    }

                    if (!match)
                        break;

                    //cout<<"matched"<<endl;
                    it->wordOffset_ = curOffset;
                    it->setStats(0, curLevel+1);
                    it++;

                    if (start >= end)
                        break;
                }
            }
            else
            {
                it++;
            }
        }
    }

    izenelib::util::UString toExpandedString(const TermList & termList)
    {
        if (termList.empty())
        {
            UString ustr;
            return ustr;
        }

        izenelib::util::UCS2Char AND_CHAR = 38; // '&' , space = 32
        izenelib::util::UCS2Char LBRACKET = 40; // '('
        izenelib::util::UCS2Char RBRACKET = 41; // ')'
        izenelib::util::UCS2Char OR_CHAR = 124; // '|'

        TermList::const_iterator it;
        unsigned int prevOffset = 0;
        int baseLevel = 0;
        int prevLevel = 0;
        unsigned char prevAndOr = 0;

        unsigned char andOrBit = 0;
        int level = 0;
        izenelib::util::UString output;

        preProcess(termList);

        for (it = termList.begin(); it != termList.end(); it++)
        {
            //cout << "for term " << *it << endl;
            andOrBit = it->getAndOrBit();
            level = it->getLevel();

            if (it == termList.begin())
            {
                output += LBRACKET;
                output += LBRACKET;
                baseLevel = level;
            }
            else
            {

                if (prevOffset < it->wordOffset_)
                {
                    ///for (int i = prevLevel; i >= baseLevel; i--)
                    ///    output += RBRACKET;
                    output += RBRACKET;
                    output += RBRACKET;
                    //if (prevLevel > level)
                    //output += RBRACKET;
                    //output += RBRACKET;
                    output += AND_CHAR;
                    output += LBRACKET;
                    output += LBRACKET;
                    baseLevel = level;
                }
                else
                {
                    if (prevLevel < level)
                    {
//                        if (prevAndOr == Term::AND)
//                            output += AND_CHAR;
//                        else if (prevAndOr == Term::OR)
//                            output += OR_CHAR;
//                        output += LBRACKET;
                        output += RBRACKET;
                        output += OR_CHAR;
                        output += LBRACKET;
                    }
                    else
                    {
//                        if (prevLevel > level)
//                            output += RBRACKET;
                        if (andOrBit == Term::AND)
                        {
                            output += AND_CHAR;
                        }
                        else if (andOrBit == Term::OR)
                        {
                            output += OR_CHAR;
                        }
                    }
                }
            }

            UString replacedText;
            replaceSpecialChar(it->text_, replacedText);
            output += replacedText;

            prevLevel = level;
            prevOffset = it->wordOffset_;
            prevAndOr = andOrBit;

            //output.displayStringValue(izenelib::util::UString::UTF_8); cout << endl;
        }

        //for (int i = prevLevel; i >= baseLevel; i--)
        //    output += RBRACKET;
        output += RBRACKET;
        output += RBRACKET;

        return output;
    }

    void LA::removeStopwords(
            TermList & termList,
            boost::shared_ptr<PlainDictionary>&  stopDict)
    {
        ScopedReadLock<ReadWriteLock> srl(stopDict->getLock());
        TermList::iterator itr = termList.begin();
        while (itr != termList.end())
        {
            //cout<<"Contains ";itr->text_.displayStringInfo(UString::UTF_8);
            //cout<<" : "<<stopDict->containNoLock(itr->text_)<<", size: "<<stopDict->size()<<endl;
            if (stopDict->containNoLock(itr->text_))
                itr = termList.erase(itr);
            else
                ++itr;
        }
    }

}
