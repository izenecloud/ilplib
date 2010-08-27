#ifndef _SBD_BASE_H_
#define _SBD_BASE_H_

#include "common.h"
#include "language.h"
#include "parameters.h"
#include "token.h"

/**
 * Base class of sbd. 
 * sbd trainer and sbd main class will inheritate 
 * from this class.
 *
 * @author Eric - 2010.08.22
 *
 */
namespace sbd
{

struct SBDBase
{

	Parameters params; // contains some statistic result
	Language language; // language related functions

    /**
     * Tokenize text as a vector of tokens.
     * Text is read line by line, the first token of a line
     * will be marked as start of line. the first token of
     * a paragraph is be marked as start of par.
     *
     */
	void tokenize(string& text, vector<Token>& tokens)
	{
		bool parastart = false;
		boost::char_separator<char> sep("\n");
		BoostTokenizer tok(text, sep);
		for (BoostTokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
		{
			string line = boost::algorithm::trim_copy(*it);
			if (!line.empty())
			{
				vector<Token> ts;
				language.tokenize(line, ts);

				// add first token
				Token t = ts[0];
				t.parastart = parastart;
				t.linestart = true;
				tokens.push_back(t);

				parastart = false;

				// add rest token
				for (size_t i=1; i<ts.size(); ++i)
				{
					tokens.push_back(ts[i]);
				}
			} else
			{
				parastart = true;
			}
		}
	}

    /**
     * Perform the first pass of annotation, which makes decisions
     * based purely on the word type of each word:
     *   - '?', '!', and '.' are marked as sentence breaks.
     *   - sequences of two or more periods are marked as ellipsis.
     *   - any word ending in '.' that's a known abbreviation is
     *     marked as an abbreviation.
     *   - any other word ending in '.' is marked as a sentence break.
     */

	void annotateFirstPass(vector<Token>& tokens)
	{
		for (size_t i=0; i<tokens.size(); ++i)
		{
			Token& token = tokens[i];
			string tok = token.tok;

			if (language.isSentenceEndChar(tok))
			{
				token.sentbreak = true;
			} else
			if (token.isEllipsis())
			{
				token.ellipsis = true;
			} else
			if (token.period_final && !boost::algorithm::ends_with(tok, ".."))
			{
				string str = tok.substr(0, tok.size()-1);
				str = boost::algorithm::to_lower_copy(str);

				vector<string> v;
				boost::algorithm::split(v, str, boost::is_any_of("-"));

				if (params.abbrev_types.find(str) != params.abbrev_types.end())
				{
					token.abbr = true;
				} else
				if (!v.empty())
				{
					string s = v[v.size()-1];
					if (params.abbrev_types.find(s) != params.abbrev_types.end())
					{
						token.abbr = true;
					} else {
						token.sentbreak = true;
					}
				} else {
					token.sentbreak = true;
				}
			}
		}
	}
};

}
#endif
