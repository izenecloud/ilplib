/*
 * =====================================================================================
 *
 *       Filename:  string_patterns.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年06月17日 17时46分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kevin Hu (), hukai.kevin@snda.com
 *        Company:  Snda.com
 *
 * =====================================================================================
 */

#ifndef _ILPLIB_NLP_STRING_PATTERNS_H_
#define _ILPLIB_NLP_STRING_PATTERNS_H_

#include <vector>
#include <string>

#include "knlp/tokenize.h"
using namespace izenelib::am::util;
using namespace izenelib;
namespace ilplib{
	namespace knlp{

class StringPatterns
{

	public:

		static int32_t string_type(const KString& kstr)
		{
			bool eng = false, num = false, chn=false, oth=false;
			for ( uint32_t i=0; i<kstr.length(); ++i)
			  if (std::isalpha(kstr[i]))
				eng = true;
			  else if(ilplib::knlp::Tokenize::is_digit(kstr[i]))
				num = true;
			  else if (KString::is_chinese(kstr[i]))
				chn = true;
			  else if (kstr[i]!=' ')
				oth = true;
			if (eng && (!num) && (!chn) && (!oth))
			  return 1;//all english char
			if (num && (!eng) && (!chn) && (!oth))
			  return 2;//all digital char
			if (eng && num && (!chn))
			  return 3;//eng and digit mixed
			if (oth && (!chn))
			  return 4;//punctuation
			return -1;
		}

};

}}
#endif

