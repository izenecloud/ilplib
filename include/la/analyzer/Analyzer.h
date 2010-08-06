/**
 * @brief   Defines Analyzerclass
 * @file    Analyzer.h
 * @author  zhjay, MyungHyun Lee (Kent)
 * @date    2009.06.10
 * @details
 *  2009.08.02 - Adding new interfaces
 */

#ifndef _LA_ANALYZER_H_
#define _LA_ANALYZER_H_

#include <la/common/Term.h>

#include <util/ustring/UString.h>

namespace la
{

    ///
    /// \brief interface of Analyzer
    /// This class analyze terms according to the specific types of analyzer
    ///
    class Analyzer
    {
        public:

            typedef int lamode;
            enum {
                prime = 0x0001,
                second = 0x0002,
                synonym = 0x0004,
                stemming = 0x0008,
                specialchar = 0x0010,
                casesensitive = 0x0100,
                containlower = 0x0100,
                nbest = 0x1000
            };

        protected:

            /// @details
            /// 00 - don't process, 01 - primary terms, 10 - secondaray terms, 11 - all terms
            unsigned char retFlag_idx_;
            unsigned char retFlag_sch_;

            /// @brief Whether enable case-sensitive search
            bool bCaseSensitive_;

            /// @brief Whether contain lower form of English
            bool bContainLower_;

        public:
            static const unsigned char ANALYZE_NONE_;
            static const unsigned char ANALYZE_PRIME_;
            static const unsigned char ANALYZE_SECOND_;
            static const unsigned char ANALYZE_ALL_;

        public:
            Analyzer()
                : retFlag_idx_( ANALYZE_SECOND_ ),
                retFlag_sch_( ANALYZE_SECOND_ ),
                bCaseSensitive_( false ),
                bContainLower_( true )
            {}

            virtual ~Analyzer(){}

            void setRetFlag_index( const unsigned char & flag )
            {
                retFlag_idx_ = flag;
            }

            void setRetFlag_search( const unsigned char & flag )
            {
                retFlag_sch_ = flag;
            }

            unsigned char getRetFlag_index() const
            {
                return retFlag_idx_;
            }

            unsigned char getRetFlag_search() const
            {
                return retFlag_sch_;
            }

            /**
             * @brief Whether enable case-sensitive search, this method only
             * set the caseSensitive flag. The children class can overwirte
             * this method.
             * @param flag default value is true
             */
            virtual void setCaseSensitive( bool flag = true )
            {
                bCaseSensitive_ = flag;
            }

            /**
             * @brief get whether the Analyzer is case sensitive
             * @return the "case sensitive" setting of this unit
             */
            bool getCaseSensitive()
            {
                return bCaseSensitive_;
            }

            /**
             * @brief Whether contain lower form of English
             * set the containLower flag. The children class can overwirte
             * this method.
             * @param flag default value is true
             */
            virtual void setContainLower( bool flag = true )
            {
                bContainLower_ = flag;
            }

            /**
             * @brief get whether the Analyzer is case sensitive
             * @return the "case sensitive" setting of this unit
             */
            bool getContainLower()
            {
                return bContainLower_;
            }

            int analyze(izenelib::ir::idmanager::IDManager* idm, const Term & input, TermIdList & output);

            int analyze_index( const TermList & input, TermList & output );

            int analyze_search( const TermList & input, TermList & output );

        protected:

            virtual int analyze(izenelib::ir::idmanager::IDManager* idm, const Term & input, TermIdList & output, unsigned char retFlag ) { return 0; }
            virtual int analyze_index( const TermList & input, TermList & output, unsigned char retFlag ){ return 0; }
            virtual int analyze_search( const TermList & input, TermList & output, unsigned char retFlag ){ return 0; }


    };
}

#endif /* _LA_ANALYZER_H_ */
