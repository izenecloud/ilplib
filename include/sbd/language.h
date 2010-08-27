#ifndef _SBD_LANG_H_
#define _SBD_LANG_H_


#include "token.h"


/**
 * stores variables, mostly regular expressions, which may be
 * language-dependent for correct application of the algorithm.
 * An extension of this class may modify its properties to suit
 * a language other than English
 *
 * @author Eric - 2010.08.22
 *
 */

namespace sbd
{

struct Language
{

	string re_sent_end_chars;
	string internal_punct;
	string re_word_start;
	string re_non_word_chars;
	string re_multi_char_punct;
	string re_word_tokenizer;
	string re_period_context;


	Language()
	{

	re_sent_end_chars = "[.?!]";

	internal_punct = ",:;";

	re_word_start = "[^\\(\\\"\\`{\\[:;&\\#\\*@\\)}\\]\\-,]";

	re_non_word_chars = "(?:[?!)\\\";}\\]\\*:@\\'\\({\\[])";

	re_multi_char_punct = "(?:\\-{2,}|\\.{2,}|(?:\\.\\s){2,}\\.)";

	re_word_tokenizer = re_multi_char_punct + "|(?=" + re_word_start + ")\\S+?(?=\\s|$|"
							+ re_non_word_chars + "|" + re_multi_char_punct + "|,(?=$|\\s|"
							+ re_non_word_chars +  "|"+ re_multi_char_punct + "))|\\S";

//	re_period_context =  "\\S*" + re_sent_end_chars + "(?=(?<after_tok>"
//							+ re_non_word_chars + "|\\s+(?<next_tok>\\S+)))";
	re_period_context =  "\\S*" + re_sent_end_chars + "(?=("
							+ re_non_word_chars + "|\\s+(\\S+)))";

	}

	bool isNonWordChar(string& tok)
	{
		if(tok.size() > 1) 
			return false;

		string nonWordChars = "?!)\\\";}]*:@'({[";
		if(nonWordChars.find(tok) != string::npos) 
			return true;

		return false;
	}

	void tokenize(string& text, vector<Token>& tokens)
	{
		boost::regex re(re_word_tokenizer);
		boost::sregex_iterator i(text.begin(), text.end(), re);
		boost::sregex_iterator j;

		while(i != j)
		{
			string tok = (*i).str();
			tokens.push_back(Token(tok));
			i++;
		}
	}

	
	bool isSentenceEndChar(string tok)
	{
		if(tok == "." || tok == "!" || tok == "?")
		{
			return true;
		}
		return false;
	}
};
}

#endif
