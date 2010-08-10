#ifndef _KOREAN_ANALYZER_H_
#define _KOREAN_ANALYZER_H_

#include <la/analyzer/CommonLanguageAnalyzer.h>
#include <la/analyzer/KoreanLanguageAction.h>


#define _CLA_INSERT_INDEX_USTR( term_it, termList, text, wordOffset, pos, morpheme ) \
    term_it = termList.insert( termList.end(), globalNewTerm_ ); \
    term_it->text_ = text; \
    term_it->wordOffset_ = wordOffset; \
    term_it->pos_ = pos; \
    term_it->morpheme_ = morpheme

#define _CLA_INSERT_INDEX_STR( term_it, termList, text, wordOffset, pos, morpheme ) \
    term_it = termList.insert( termList.end(), globalNewTerm_ ); \
    term_it->text_.assign( text, encode_ ); \
    term_it->wordOffset_ = wordOffset; \
    term_it->pos_ = pos; \
    term_it->morpheme_ = morpheme

namespace la
{
class KoreanAnalyzer : public CommonLanguageAnalyzer<KoreanLanguageAction, kmaOrange::WK_Eojul>
{

public:


    /**
     * @brief   Sets whether to generate compound noun with two adjacent extracted nouns
     *          (only indexing)
     */
    inline void setGenerateCompNoun( bool flag=false )
    {
        bGenCompNoun_ = flag;
    }

    /**
     * @brief   Whether to combine number dependent noun to extracted term
     */
    inline void setCombineBoundNoun( bool flag=false )
    {
        lat_->setCombineBoundNoun( flag );
    }


    // --- SPECIALCHAR ---

    /**
     * @brief   Set special characters to process. The configured characters, of course,
     *          should be passed to KoreanAnalyzer. (normall with "allow" option to Tokenizer)
     */
    void setSpecialChars( const string & chars )
    {
        const char *o = chars.c_str();

        // init special chars table
        memset(specialCharTable_, 0x00, sizeof(char) * 128);

        if (*o)
        {
            // turn on special chars option.
            bSpecialChars_ = true;

            while (*o)
            {
                if( (unsigned char)*o < 128 )
                {
                    specialCharTable_[(unsigned char)*o] = 1;
                }
                o++;
            }
        }
    }

protected:


    inline void combineSpecialChar( kmaOrange::WK_Eojul * pE, int i, int count, TermList& termList )
    {
        TermList::iterator term_it;
        string specialStr;
        int k;
        for( int j = 0; j < count; ++j )
        {
            unsigned int pos = pE->getPOS(i,j);
            if( ( pos & scMorp_ ) == pos )
            {
                if( (k = getSpecialCharsString(pE, i, j, specialStr)) >= 0 )
                {
                    _CLA_INSERT_INDEX_STR( term_it, termList, specialStr, k, nniPOS_, nniMorp_ );
                    j = k;
                }
            }
        }
    }


    int getSpecialCharsString(
        kmaOrange::WK_Eojul * pEojul,
        int listi, int counti,
        string & specialStr );

    void generateCompundNouns(
        kmaOrange::WK_Eojul * pE,
        const string& inputstr,
        int i, int count,
        unsigned int wordOffset,
        TermList& termList );

    void addChineseTerm(
        const char * pTerm,
        const unsigned int wordOffset,
        TermList & tlist );

private:

    int scMorp_; // For KMA, it is kmaOrange::SC, special character

    // ---- RELATED TO "specialchar" IN LAMANAGER
    /// @brief  Whether to handle special characters.
    /// Adjacent alphabets, numeric characters, and special characters will be concatenated.
    bool bSpecialChars_;

    /// @brief  Table for special characters that should be considered for analyzing.
    char specialCharTable_[128];


    /// @brief  Genearte compound noun out of two adjacent extracted noun terms
    /// Default: false
    bool            bGenCompNoun_;


    /// @brief  Can choose the number of possible extractions to generate.
    /// Default: 2
    //unsigned int    nBest_;


    /// @brief  Whether to add Chinese characters additional to their Korean counterpart
    /// Default: false
    bool            bExtractChinese_;     //H


    Term globalNewTerm_;

};
}

#endif
