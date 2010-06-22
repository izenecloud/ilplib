/// @file Stemmer.h
/// @brief header file of Stemmer
/// @author JunHui
/// @date 2008-07-29

#ifndef _LA_STEMMER_H_
#define _LA_STEMMER_H_

#include <string>
#include <la/stem/api.h>

//using namespace std;

namespace la
{
namespace stem
{
    enum Language
    {
        STEM_LANG_UNKNOWN = 0,
        STEM_LANG_DANISH = 1,
        STEM_LANG_DUTCH = 2,
        STEM_LANG_ENGLISH,
        STEM_LANG_FINNISH,
        STEM_LANG_FRENCH,
        STEM_LANG_GERMAN,
        STEM_LANG_HUNGARIAN,
        STEM_LANG_ITALIAN,
        STEM_LANG_NORWEGIAN,
        STEM_LANG_PORTUGUESE,
        STEM_LANG_ROMANIAN,
        STEM_LANG_RUSSIAN,
        STEM_LANG_SPANISH,
        STEM_LANG_SWEDISH,
        STEM_LANG_TURKISH,
        STEM_LANG_EOS,
    } ;
    ///
    /// @brief stem a word
    ///
    class Stemmer
    {
        public:
            // stemming support language
            ///
            /// constructor.
            ///
            Stemmer();

            ///
            /// destructor.
            ///
            virtual ~Stemmer();

            ///
            /// Initialize supported language.
            /// @param language stemming language, value is one of StemLanguage enumeration
            /// @return if success true, else false.
            ///
            bool init(Language language);

            ///
            /// delete stemming function
            /// @param void
            /// @return void
            ///
            void deinit(void);

            ///
            /// perform stemming
            /// @param term word for stemming
            /// @param resultWord the result word after stemming
            /// @param if success true, else false.
            ///
            bool stem(const std::string& term, std::string& resultWord);

        private:
            //int stemLang_; ///< language for stemming

            void* stemFunction_; ///< stemming function
    };

}}
#endif // _LA_STEMMER_H_
