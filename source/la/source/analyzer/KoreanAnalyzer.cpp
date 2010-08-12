#include <la/analyzer/KoreanAnalyzer.h>

using namespace kmaOrange;

namespace la
{


    bool KoreanAnalyzer::isScFlSn( int morp )
    {
        static int SC_FL_SN_TAGS = (SC|FL|SN);
        return morp & SC_FL_SN_TAGS == morp;
    }


    bool KoreanAnalyzer::isAcceptedNoun( int morp )
    {
        static int sTagSetNoun = (NNG|NFG|NNB|NNP|NNU|NNR|NP|NU|NNI|NNC|NFU); //(N_|UW);
        return (morp & sTagSetNoun ) == morp;
    }


void KoreanAnalyzer::generateCompundNouns(
        WK_Eojul * pE, const string& inputstr, int i, int count,
        unsigned int wordOffset, TermList& termList )
{
    unsigned int pos = 0;
    TermList::iterator term_it;
    for ( int j = 0; j < count; j++ )
    {
        const char* lexicon = pE->getLexicon( i, j );
        if( strlen(lexicon) == 0 )
            continue;

        pos = pE->getPOS(i,j);

        if( isAcceptedNoun( pos ) )
        {
            for( int k = j+1; k <count; k++ )
            {
                pos = pE->getPOS(i,k);
                if( isAcceptedNoun( pos ) )
                {
                    string tstr( lexicon );
                    tstr.append( pE->getLexicon(i,k) );

                    if( tstr == inputstr )
                        continue;

                    _CLA_INSERT_INDEX_STR( term_it, termList, tstr,
                                           wordOffset, nnpPOS_, nnpMorp_ );
                }
            }
        }
    }
}

int KoreanAnalyzer::getSpecialCharsString(
    WK_Eojul * pEojul,
    int listi, int counti,
    string & specialStr )
{
    const char *tmpStr;
    int  tmpLen, morphCount;

    tmpStr     = pEojul->getLexicon(listi, counti);
    tmpLen     = strlen(tmpStr);
    morphCount = pEojul->getCount(listi);

    for (int z = 0; z < tmpLen; z++)
    {
        if (!(tmpStr[z] & 0x80) && specialCharTable_[(unsigned char)tmpStr[z]])
        {
            int pi, ni;

            // find start index
            if (counti <= 0) pi = 0;
            else
            {
                for (pi = counti - 1; pi >= 0; pi--)
                {
                    if( isScFlSn( pEojul->getPOS(listi, pi) ) )
                    {
                        break;
                    }
                }
                pi++;
            }

            // find end index
            if (counti >= (morphCount-1)) ni = morphCount-1;
            else
            {
                for (ni = counti + 1; ni < morphCount; ni++)
                {
                    if( isScFlSn( pEojul->getPOS(listi, pi) ) )
                    {
                        break;
                    }
                }
                ni--;
            }

            // validation
            if (pi == ni) return -1;

            // make term
            specialStr = pEojul->getLexicon(listi, pi);
            //strcpy(pSCString, pEojul->getLexicon(listi, pi));
            for (z = pi + 1; z <= ni; z++)
            {
                //strcat(pSCString, pEojul->getLexicon(listi, z));
                specialStr.append(pEojul->getLexicon(listi, z));
            }
            return ni;
        }
    }
    return -1;
}

void KoreanAnalyzer::addChineseTerm(
    const char * pTerm,
    const unsigned int wordOffset,
    TermList & tlist )
{
    TermList::iterator term_it;

    // for string
    const char* h   = pTerm;
    int   len = strlen(pTerm);
    int   sb = -1, eb = -1;

    // for check hanja
    unsigned char hb, lb;

    // for other term
    //char hanja_term[1024+1];
    char * hanja_term = new char[len+1];
    if( hanja_term == NULL )
        return;

    memset( hanja_term, 0, len+1 );

    int i;

    for (i = 0; i < len; i++, h++)
    {
        if (*h & 0x80)
        {
            hb = (unsigned char)*h;
            h++;
            i++;

            if (!*h) break;

            lb = (unsigned char)*h;

            // is hanja(Chinese)?
            if ((hb >= 0xCA && hb <= 0xFD) && (lb >= 0xA1 && lb <= 0xFE))
            {
                if (sb == -1) sb = i-1;
                eb = i;

                // check next char
                continue;
            }
        }

        // Chinese character check condition
        if (sb >= 0 && (eb > sb))
        {
            strncpy(hanja_term, pTerm + sb, eb-sb+1);
            hanja_term[eb-sb+1] = 0;
            _CLA_INSERT_INDEX_STR( term_it, tlist, hanja_term, wordOffset, flPOS_, flMorp_ );

            sb = -1;
            eb = -1;
        }
    }

    // Chinese character check condition
    if (sb >= 0 && (eb > sb))
    {
        strncpy(hanja_term, pTerm + sb, eb-sb+1);
        hanja_term[eb-sb+1] = 0;
        _CLA_INSERT_INDEX_STR( term_it, tlist, hanja_term, wordOffset, flPOS_, flMorp_ );

        sb = -1;
        eb = -1;
    }

    delete[] hanja_term;
}

}
