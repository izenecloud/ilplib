#include <la/stem/Stemmer.h>

#include <la/stem/stem_UTF_8_danish.h>
#include <la/stem/stem_UTF_8_dutch.h>
#include <la/stem/stem_UTF_8_english.h>
#include <la/stem/stem_UTF_8_finnish.h>
#include <la/stem/stem_UTF_8_french.h>
#include <la/stem/stem_UTF_8_german.h>
#include <la/stem/stem_UTF_8_hungarian.h>
#include <la/stem/stem_UTF_8_italian.h>
#include <la/stem/stem_UTF_8_norwegian.h>
#include <la/stem/stem_UTF_8_portuguese.h>
#include <la/stem/stem_UTF_8_romanian.h>
#include <la/stem/stem_UTF_8_russian.h>
#include <la/stem/stem_UTF_8_spanish.h>
#include <la/stem/stem_UTF_8_swedish.h>
#include <la/stem/stem_UTF_8_turkish.h>


using namespace std;

namespace la
{
namespace stem
{

#ifdef __cplusplus

    extern "C" {
#endif
        struct StemFunc
        {

            struct SN_env* (*create)(void);
            void (*close)(struct SN_env*);
            int (*stem)(struct SN_env*);

            struct SN_env* env;
        };


#ifdef __cplusplus
    }
#endif



    StemFunc STEM_FUNCTION[STEM_LANG_EOS] =
    {
        {0,                           0,                          0,                     0},
        {danish_UTF_8_create_env,     danish_UTF_8_close_env,     danish_UTF_8_stem,     0},
        {dutch_UTF_8_create_env,      dutch_UTF_8_close_env,      dutch_UTF_8_stem,      0},
        {english_UTF_8_create_env,    english_UTF_8_close_env,    english_UTF_8_stem,    0},
        {finnish_UTF_8_create_env,    finnish_UTF_8_close_env,    finnish_UTF_8_stem,    0},
        {french_UTF_8_create_env,     french_UTF_8_close_env,     french_UTF_8_stem,     0},
        {german_UTF_8_create_env,     german_UTF_8_close_env,     german_UTF_8_stem,     0},
        {hungarian_UTF_8_create_env,  hungarian_UTF_8_close_env,  hungarian_UTF_8_stem,  0},
        {italian_UTF_8_create_env,    italian_UTF_8_close_env,    italian_UTF_8_stem,    0},
        {norwegian_UTF_8_create_env,  norwegian_UTF_8_close_env,  norwegian_UTF_8_stem,  0},
        {portuguese_UTF_8_create_env, portuguese_UTF_8_close_env, portuguese_UTF_8_stem, 0},
        {romanian_UTF_8_create_env,   romanian_UTF_8_close_env,   romanian_UTF_8_stem,   0},
        {russian_UTF_8_create_env,    russian_UTF_8_close_env,    russian_UTF_8_stem,    0},
        {spanish_UTF_8_create_env,    spanish_UTF_8_close_env,    spanish_UTF_8_stem,    0},
        {swedish_UTF_8_create_env,    swedish_UTF_8_close_env,    swedish_UTF_8_stem,    0},
        {turkish_UTF_8_create_env,    turkish_UTF_8_close_env,    turkish_UTF_8_stem,    0},
    };

    Stemmer::Stemmer()
    {
        //stemLang_ = STEM_LANG_UNKNOWN;
        stemFunction_ = 0;
    }

    Stemmer::~Stemmer()
    {
        deinit();
    }

    bool Stemmer::init(Language language)
    {
        // create stemming function structure
        stemFunction_ = (void*)new StemFunc;
        if (stemFunction_ == 0)
        {
            return false;
        }

        // set stemming functions
        if( language > 0 && language < STEM_LANG_EOS )
        {
            ((StemFunc*)stemFunction_)->create = STEM_FUNCTION[language].create;
            ((StemFunc*)stemFunction_)->close  = STEM_FUNCTION[language].close;
            ((StemFunc*)stemFunction_)->stem   = STEM_FUNCTION[language].stem;
            ((StemFunc*)stemFunction_)->env    = STEM_FUNCTION[language].env;
        }
        else
        {
            delete (StemFunc*)stemFunction_;
            stemFunction_= 0;
            return false;
        }

        // create env
        ((StemFunc*)stemFunction_)->env = ((StemFunc*)stemFunction_)->create();
        if (((StemFunc*)stemFunction_)->env == 0)
        {
            deinit();
            return false;
        }

        return true;
    }
    ////////////
        struct SN_env {
            symbol * p;
            int c; int l; int lb; int bra; int ket;
            symbol * * S;
            int * I;
            unsigned char * B;
        };
    ////////////

    void Stemmer::deinit(void)
    {
        if(stemFunction_)
        {
            ((StemFunc*)stemFunction_)->close(((StemFunc*)stemFunction_)->env);
            delete (StemFunc*)stemFunction_;
            stemFunction_ = 0;
        }
    }

    bool Stemmer::stem(const string& term, string& resultWord)
    {
        if(!stemFunction_)
        {
            return false;
        }

        // set environment
        if (SN_set_current(((StemFunc*)stemFunction_)->env, term.length(), (const symbol*)term.c_str()))
        {
            ((StemFunc*)stemFunction_)->env->l = 0;
            return false;
        }

        // stemming
        if (((StemFunc*)stemFunction_)->stem(((StemFunc*)stemFunction_)->env) < 0)
        {
            return false;
        }

        ((StemFunc*)stemFunction_)->env->p[((StemFunc*)stemFunction_)->env->l] = 0;

        resultWord = (char*)((StemFunc*)stemFunction_)->env->p;

        return true;
    }

}}
