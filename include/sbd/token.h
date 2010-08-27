#ifndef _SBD_TOKEN_H_
#define _SBD_TOKEN_H_

#include <iostream>
#include <boost/algorithm/string.hpp>
#include "common.h"

using namespace std;


/**
 * Stores a token of text with annotations produced during
 * sentence boundary detection.
 *
 * @author Eric - 2010.08.22
 * 
 */
namespace sbd
{

struct Token
{
public:
	Token(string str):tok(str), period_final(false), parastart(false), linestart(false),
abbr(false), ellipsis(false), sentbreak(false)	{
		type = boost::algorithm::to_lower_copy(tok);
        // number pattern
		boost::regex reg("^-?[\\.,]?\\d[\\d,\\.-]*\\.?$");
        // any number is belong to the same type
		type = boost::regex_replace(type, reg, "##number##");
		period_final = boost::algorithm::ends_with(tok, PERIOD);
	}

	string tok; // original string
	string type; // lower case, number is uniformed as "##number##"

	bool period_final;
	bool parastart;
	bool linestart;
	bool abbr;
	bool ellipsis;
	bool sentbreak;


    /**
     * token type without period
     */
	string typeNoPeriod()
	{
		if (type.size() > 1 && type[type.size()-1] == '.')
		{
			return type.substr(0, type.size()-1);
		} else {
			return type;
		}
	}

    /**
     * token type without period if the token 
     * is sentence break.
     * 
     */
	string typeNoSentperiod()
	{
		if (sentbreak)
		{
			return typeNoPeriod();
		}
		return type;
	}

	bool isFirstUpper()
	{
		if (tok.empty())
		{
			return false;
		}
		char c = tok[0];
		return c >= 65 && c <= 90;
	}

	bool isFirstLower()
	{
		if (tok.empty())
		{
			return false;
		}
		char c = tok[0];
		return c >= 97 && c <= 122;
	}

	string firstCase()
	{
		if(isFirstLower())
		{
			return LOWER;
		}
		else
		if (isFirstUpper())
		{
			return UPPER;
		}
		return NONE;
	}

	bool isEllipsis()
	{
		boost::regex expr("^\\.\\.+$");
		return boost::regex_match(tok, expr);
	}

	bool isNumber()
	{
//		boost::regex expr("^-?[\\.,]?\\d[\\d,\\.-]*\\.?");
//		return boost::regex_match(token, expr);

		return boost::algorithm::starts_with(type, "##number##");
	}

	bool isInitial()
	{
		boost::regex expr("[^\\W\\d]\\.$");
		return boost::regex_match(tok, expr);
	}

	bool isAlpha()
	{
		boost::regex expr("[^\\W\\d]+$");
		return boost::regex_match(tok, expr);
	}

	bool isNonPunct()
	{
		boost::regex expr("[^\\W\\d]");
		boost::smatch what;
		return boost::regex_search(type, what, expr);
	}

	bool endsWithPeriod() {
		return period_final;
	}

	string toString()
	{
		string res = tok;
		if (abbr)
		{
			res += "<A>";
		}
		if (ellipsis)
		{
			res += "<E>";
		}
		if (sentbreak)
		{
			res += "<S>";
		}
        return res;
	}

};
}

#endif
