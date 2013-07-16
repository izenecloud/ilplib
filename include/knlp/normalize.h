/*
 * =====================================================================================
 *
 *       Filename:  normalize.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2013年05月22日 12时04分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), kevin.hu@b5m.com
 *        Company:  B5M.com
 *
 * =====================================================================================
 */
#ifndef _ILPLIB_NLP_NORMALIZE_H_
#define _ILPLIB_NLP_NORMALIZE_H_

#include "trd2simp.h"
#include "util/string/kstring.hpp"

#include <string>

namespace ilplib
{
namespace knlp
{

class Normalize
{
    static Trad2Simp trd2smp_;
public:
    static void normalize(izenelib::util::KString& kstr)
    {
        kstr.to_dbc();
        kstr.to_lower_case();
        trd2smp_.transform(kstr);
        kstr.trim_into_1();
        kstr.trim_head_tail();
    }
	
	static void normalize(std::string& str)
    {
		izenelib::util::KString kstr(str);
		normalize(kstr);
		str = kstr.get_bytes("utf-8");
    }
};

}
}//namespace
#endif
