#include <la/util/UStringUtil.h>

using namespace izenelib::util;
using namespace std;

namespace la
{

    std::string to_utf8(std::string input, izenelib::util::UString::EncodingType encode)
    {
        UString ustr(input, encode);
        string str;
        ustr.convertString(str, UString::UTF_8);
        return str;
    }

    const UString SPACE_USTR(" ", UString::UTF_8 );

    void removeRedundantSpaces(
            const izenelib::util::UString& input,
            izenelib::util::UString& output
            )
    {
        using namespace izenelib::util;
        UString tmpOutput;

        bool lastIsCn = false; // last is Chinese Character
        bool coverSpace = false; // whether traverse space last time

        size_t len = input.length();

        for( size_t i = 0; i < len; ++i )
        {
            // set the current mode
            UCS2Char curChar = input[ i ];
            if( UString::isThisSpaceChar( curChar ) )
            {
                coverSpace = true;
                continue;
            }

            bool curIsCn = UString::isThisChineseChar( curChar );

            if( coverSpace == true )
            {
                if( curIsCn == false || lastIsCn == false )
                {
                    tmpOutput += SPACE_USTR;
                }
                coverSpace = false;
            }

            tmpOutput += curChar;
            lastIsCn = curIsCn;
        }
        output.swap( tmpOutput );
    }

    void removeRedundantSpaces( izenelib::util::UString& output)
    {
        removeRedundantSpaces( output, output );
    }
}
