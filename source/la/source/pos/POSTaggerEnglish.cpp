#include <pos/POSTaggerEnglish.h>

namespace la
{
    /*
    void POSTaggerEnglish::tag( Term & term )
    {
        Token t( term->textString(), "?" );

        const multimap<std::string, std::string> dummy;

        //bidir_decode_beam(vt, dummy, vme_);
        bidir_decode_beam(t, dummy, vme_);
            
        term.pos_ = vt[i].prd;;
    }

    void POSTaggerEnglish::tag(TermList & input)
    {
        TermList::iterator it;
        for( it = input.begin(); it != input.end(); it++ )
        {
            tag( *it );
        }
    }
    */

    void POSTaggerEnglish::tag(TermList & input)
    {
        std::vector<Token> vt;

        TermList::iterator it = input.begin();
        for(; it != input.end() ; it++)
            vt.push_back(Token(it->textString(), "?"));

        const multimap<std::string, std::string> dummy;

        bidir_decode_beam(vt, dummy, vme_);

        it = input.begin();

        for (size_t i = 0; i < vt.size(); i++,it++) 
        {
            std::string p = vt[i].prd;
            it->pos_ = p;
        }
    }

    void POSTaggerEnglish::tag(const TermList & input, TermList & output )
    {
        std::vector<Token> vt;

        TermList::const_iterator it = input.begin();
        for(; it != input.end() ; it++)
            vt.push_back(Token(it->textString(), "?"));

        const multimap<std::string, std::string> dummy;

        bidir_decode_beam(vt, dummy, vme_);

        output = input;
        TermList::iterator it2 = output.begin();

        for (size_t i = 0; i < vt.size(); i++,it2++) 
        {
            it2->pos_ = vt[i].prd;
        }
    }
}
