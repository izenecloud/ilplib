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

            izenelib::util::UString    text_;

            std::string         pos_;

            unsigned int        wordOffset_;

        public:

            Term() : pos_("@"), wordOffset_(0) {}

            Term(const izenelib::util::UString & str)
                : text_(str), pos_("@"), wordOffset_(0) {}

            ~Term() {}

            void clear()
            {
                text_.clear();
                wordOffset_ = 0;
            }


            std::string toString() const;

            // TODO: remove? or handle hard-coded encoding
            std::string textString() const
            {
                std::string tmp;
                text_.convertString(tmp, izenelib::util::UString::UTF_8);
                return tmp;
            }

            int length()
            {
                return text_.length();
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

            inline void add( const UString & ustr, const unsigned int offset ) {
                push_back(globalTemporary_);
                back().text_ = ustr;
                back().wordOffset_ = offset;
            }

        private:

            static Term globalTemporary_;
    };

    std::ostream & operator<<( std::ostream & out, TermList & termList );

    inline bool wordOffset_sorter( const Term & t1, const Term & t2 )
    {
        return ( t1.wordOffset_ < t2.wordOffset_ );
    }

    void sortByWordOffset( TermList & termList );

    void appendPlaceHolder( TermList & termList, TermList & placeHolder );

    extern UString PLACE_HOLDER;


}

#endif /* _LA_TERM_H_ */
