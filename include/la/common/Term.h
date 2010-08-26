/**
 * @brief   Defines Term, TermNode class
 * @file    Term.h
 * @author  zhjay, MyungHyun Lee (Kent)
 * @date    2009.06.10
 * @details
 *  2009.08.02 - Add new class TermNode used int Analyzer
 */

#ifndef _LA_TERM_H_
#define _LA_TERM_H_


#include <util/ustring/UString.h>
#include <ir/id_manager/IDManager.h>
#include <ir/index_manager/index/LAInput.h>

#include <boost/serialization/access.hpp>
#include <boost/function.hpp>

#include <sstream>
#include <ostream>
#include <list>
#include <deque>
#include <bitset>

using namespace izenelib::ir::indexmanager;
namespace la
{

class Term
{
public:

    static const unsigned char OR;
    static const unsigned char AND;

    izenelib::util::UString    text_;

    unsigned int        wordOffset_;

    ///  pos tag
    std::string         pos_;

    /// @brief  Saves the information of the extraction
    /// @details
    ///  1: and(0)/or(1) term
    ///  2-8: represents level in an integer value
    ///  ONLY USED IN SEARCH RELATED METHODS
    unsigned char       stats_;

public:

    Term() : wordOffset_(0), pos_("?"), stats_(0) {}

    Term(const izenelib::util::UString & str)
        : text_(str), wordOffset_(0), pos_("?"), stats_(0) {}

    ~Term() {}

    void clear()
    {
        text_.clear();
        wordOffset_ = 0;
        pos_ = "?";
        stats_ = 0;
    }

    inline void setStats( unsigned char andOrBit, unsigned int level  )
    {
        stats_ = ((andOrBit&0x01)<<7)|((unsigned char)(level)&0x7F);
    }

    inline void getStats( unsigned char & andOrBit, unsigned int & level) const
    {
        andOrBit = (stats_ & 0x80) >> 7;
        level = (unsigned int)(stats_ & 0x7F);
    }

    inline unsigned char getAndOrBit() const {
        return (stats_ & 0x80) >> 7;
    }

    inline unsigned int getLevel() const {
        return (unsigned int)(stats_ & 0x7F);
    }

    int length()
    {
        return text_.length();
    }

    std::string toString() const;

    // TODO: remove? or handle hard-coded encoding
    std::string textString() const
    {
        std::string tmp;
        text_.convertString(tmp, izenelib::util::UString::UTF_8);
        return tmp;
    }

    friend std::ostream & operator<<( std::ostream & out, const Term & term );

private:

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& text_;
        ar& wordOffset_;
    }

};

class TermList : public  std::deque<Term>
{
public:

    inline void add(
        const UString::CharT* text,
        const size_t len,
        const unsigned int offset,
        const char * pos,
        const unsigned char andOrBit,
        const int level )
    {
        push_back(globalTemporary_);
        back().text_.assign(len, text);
        back().wordOffset_ = offset;
        if(pos) {
            back().pos_.assign(pos);
        }
        back().setStats(andOrBit, level);
    }

private:

    static Term globalTemporary_;
};

std::ostream & operator<<( std::ostream & out, TermList & termList );

//inline bool wordOffset_sorter( const Term & t1, const Term & t2 )
//{
//    return ( t1.wordOffset_ < t2.wordOffset_ );
//}
//
//void sortByWordOffset( TermList & termList );
//
//void appendPlaceHolder( TermList & termList, TermList & placeHolder );

extern UString PLACE_HOLDER;


}

#endif /* _LA_TERM_H_ */
