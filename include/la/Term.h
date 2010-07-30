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

#include <boost/serialization/access.hpp>
#include <boost/function.hpp>

#include <sstream>
#include <ostream>
#include <list>
#include <deque>
#include <bitset>


namespace la
{

    ///
    /// \brief term information class
    /// Term is the output unit of LA manager, This class record the basic information of a term
    ///
    class Term
    {
        public:

            // aligned the variables in size order
            izenelib::util::UString    text_;          ///< text data of the term
            std::string         pos_;           ///< pos tag
            unsigned int        wordOffset_;    ///< word offset of the term in original text
            int                 morpheme_;      ///< morpheme informations (for korean)

            /// @brief  Saves the information of the extraction
            /// @details
            ///  1: and(0)/or(1) term
            ///  2-8: represents level in an integer value
            ///  ONLY USED IN SEARCH RELATED METHODS
            unsigned char       stats_;

            /// @brief  stop bit
            ///  ONLY USED IN INDEX RELATED METHODS
            unsigned char       stopBit_;     //TODO: DEFAULT VALUE??


            static const unsigned char OR_BIT;
            static const unsigned char AND_BIT;

        public:

            Term()
                :   pos_("?")
                    ,wordOffset_(0)
                    ,morpheme_(0)
                    ,stats_(0)
            {}

            Term(const izenelib::util::UString & str)
                :   text_(str),
                    pos_("?")
                    ,wordOffset_(0)
                    ,morpheme_(0)
                    ,stats_(0)
            {}

            ~Term()
            {}

            void clear()
            { text_.clear();
                pos_ = "?";
                wordOffset_ = 0;
                morpheme_ = 0;
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
                ar& pos_;
                ar& morpheme_;
            }

    };

    inline unsigned char makeStatBit( unsigned char andOrBit, unsigned char level  )
    {
        unsigned char temp = 0;

        andOrBit &= 0x01;
        andOrBit = andOrBit << 7;
        level &= 0x7F;

        temp |= andOrBit;
        temp |= level;

        return temp;
    }

    inline void readStatBit( const unsigned char & stat, unsigned char & andOrBit, unsigned char & level )
    {
        andOrBit = (stat & 0x80);
        andOrBit = andOrBit >> 7;

        level = (stat & 0x7F);

        return;
    }

    typedef std::list<Term> TermList;

    std::ostream & operator<<( std::ostream & out, TermList & termList );

    inline bool wordOffset_sorter( const Term & t1, const Term & t2 )
    {
        return ( t1.wordOffset_ < t2.wordOffset_ );
    }

    void sortByWordOffset( TermList & termList );

    void appendPlaceHolder( TermList & termList, TermList & placeHolder );


    class TermId
    {
        public:

            unsigned int     termid_;
            unsigned int     wordOffset_;

            friend std::ostream & operator<<( std::ostream & out, const TermId & term );

        private:

            friend class boost::serialization::access;
            template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
            {
                ar& termid_;
                ar& wordOffset_;
            }
    };

    class UStringHashFunctor {
        public:
            virtual ~UStringHashFunctor() = 0;
            virtual bool operator()(const izenelib::util::UString& key, unsigned int& value) = 0;
    };

    template<typename IDManagerType>
    class IDManagerUStringHashFunctor : public UStringHashFunctor {
        public:
            IDManagerUStringHashFunctor(IDManagerType* idManager) : idManager_(idManager){}
            ~IDManagerUStringHashFunctor(){}

            bool operator()(const izenelib::util::UString& key, unsigned int& value)
            {
                return idManager_->getTermIdByTermString(key, value);
            }

        private:
            IDManagerType* idManager_;
    };

    typedef std::deque<TermId> TermIdList;

}

#endif /* _LA_TERM_H_ */
