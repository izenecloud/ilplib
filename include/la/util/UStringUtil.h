#ifndef _LA_USTRING_UTIL_H_
#define _LA_USTRING_UTIL_H_

#include <util/ustring/UString.h>

namespace la
{
    /**
     * Remove redundant spaces in the input string
     */
    void removeRedundantSpaces(
            const izenelib::util::UString& input,
            izenelib::util::UString& output
            );

    void removeRedundantSpaces( izenelib::util::UString& output );
}

#endif
