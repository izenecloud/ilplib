#include <la/util/UStringUtil.h>

#include <iomanip>
#include <string>

using namespace izenelib::util;
using namespace std;

namespace la
{
	// number and alpha characters
	const unsigned char UCS_NUM_ALPHA_FULL2WIDTH_2ND_BYTE[ 256 ] = {
	//  0x0 0x1 0x2 0x3 0x4 0x5 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x00
		  0x30,  0x31,  0x32,  0x33,  0x34,  0x35,  0x36,  0x37,  0x38,  0x39,  0,  0,  0,  0,  0,  0, // 0x10, 0~9
		  0,  0x41,  0x42,  0x43,  0x44,  0x45,  0x46,  0x47,  0x48,  0x49,  0x4a,  0x4b,  0x4c,  0x4d,  0x4e,  0x4f, // 0x20, A~Z
		  0x50,  0x51,  0x52,  0x53,  0x54,  0x55,  0x56,  0x57,  0x58,  0x59,  0x5a,  0,  0,  0,  0,  0, // 0x30
		  0,  0x61,  0x62,  0x63,  0x64,  0x65,  0x66,  0x67,  0x68,  0x69,  0x6a,  0x6b,  0x6c,  0x6d,  0x6e,  0x6f, // 0x40, a~z
		  0x70,  0x71,  0x72,  0x73,  0x74,  0x75,  0x76,  0x77,  0x78,  0x79,  0x7a,  0,  0,  0,  0,  0, // 0x50
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x60
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x70
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x80
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x90
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xa0
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xb0
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xc0
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xd0
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0xe0
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  // 0xf0
	};

    std::string to_utf8(izenelib::util::UString input)
    {
        string str;
        input.convertString(str, UString::UTF_8);
        return str;
    }

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

    void convertFull2HalfWidth(izenelib::util::UString& input)
    {
    	size_t len = input.length();

    	UCS2Char byte1, byte2;
    	for (size_t i = 0; i < len; i++)
    	{
    		byte1 = input[i] >> 8;
    		byte2 = input[i] & 0x00ff;

    		if (byte1 == 0xff && UCS_NUM_ALPHA_FULL2WIDTH_2ND_BYTE[ byte2 ]) {
    			input[i] = UCS_NUM_ALPHA_FULL2WIDTH_2ND_BYTE[ byte2 ];
    		}
    	}
    }
}

