#ifndef _LA_USTRING_UTIL_H_
#define _LA_USTRING_UTIL_H_

#include <string>
#include <util/ustring/UString.h>

namespace la
{
    std::string to_utf8(izenelib::util::UString input);

    std::string to_utf8(std::string input, izenelib::util::UString::EncodingType encode);

    /**
     * Remove redundant spaces in the input string
     */
    void removeRedundantSpaces(
            const izenelib::util::UString& input,
            izenelib::util::UString& output
            );

    void removeRedundantSpaces( izenelib::util::UString& output );

    /**
     * Convert full-width of numbers and alphabet characters to half-width
     */
    void convertFull2HalfWidth(izenelib::util::UString& input);
}

#endif
