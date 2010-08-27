#ifndef _SBD_H_
#define _SBD_H_

#include "sbd_base.h"
#include "sbd_trainer.h"

/**
 * sbd main
 *
 * 
 *
 * @author Eric - 2010.08.22
 */
namespace sbd
{

class SBD : public SBDBase
{
	
public:


	void train(string& text)
	{
		SBDTrainer trainer;
		trainer.train(text);
		params = trainer.getParams();
	}

	void extractSentences(string& text, vector<string>& sentences)
	{
		slice(text, sentences);
	}

private:

	void slice(string& text, vector<string>& sentences)	
	{
		boost::regex re(language.re_period_context);
		boost::sregex_iterator i(text.begin(), text.end(), re);
		boost::sregex_iterator j;
		int lastBreak = 0;
		while (i!=j)
		{
			string token1 = (*i)[0].str();
			string token2 = (*i)[1].str();
			string token3 = (*i)[2].str();
			string context = token1 + token2;
//			cout << token1 << "--" << token2 << endl;
			if(containSentBreak(context))
			{
				int end = (*i)[0].second - text.begin();
				sentences.push_back(text.substr(lastBreak, end-lastBreak));
				if (language.isNonWordChar(token2))
				{
					lastBreak = end;
				}
				else
				{
					lastBreak = (*i)[2].first - text.begin();
				}
				
			}
			i++;
		}
		sentences.push_back(text.substr(lastBreak, text.size()));
				
	}


	bool containSentBreak(string& text)
	{
		bool found = false;
		vector<Token> tokens;
		tokenize(text, tokens);
		annotateTokens(tokens);
		for (size_t i=0; i<tokens.size(); ++i)
		{
			if (found) return true;
			if (tokens[i].sentbreak)
				found = true;
		}
		return false;
	}

	void annotateTokens(vector<Token>& tokens)
	{
//		cout << "====PASS 1====" << endl;
		annotateFirstPass(tokens);
//		print(tokens);
//		cout << "====PASS 2====" << endl;
		annotateSecondPass(tokens);
//		print(tokens);
	}
	
	void annotateSecondPass(vector<Token>& tokens)
	{
		for (size_t i=0; i<tokens.size()-1; ++i)
		{	
			Token& token1 = tokens[i];
			Token& token2 = tokens[i+1];
			if (!token1.period_final) continue;
			
			string tok = token1.tok;
			string type = token1.typeNoPeriod();
			string nextTok = token2.tok;
			string nextType = token2.typeNoPeriod();
			bool isInitial = token1.isInitial();
            

            // [4.1.2. Collocational Heuristic] If there's a
            // collocation between the word before and after the
            // period, then label tok as an abbreviation and NOT
            // a sentence break. Note that collocations with
            // frequent sentence starters as their second word are
            // excluded in training.
            //
			Collocation coll(type, nextType);
			if (params.collocations.find(coll) != params.collocations.end())
			{
				token1.sentbreak = false;
				token1.abbr = true;
				continue;
			}

            // [4.2. Token-Based Reclassification of Abbreviations] If
            // the token is an abbreviation or an ellipsis, then decide
            // whether we should *also* classify it as a sentbreak.
            //
			if ( (token1.abbr || token1.ellipsis) && !isInitial )
			{
				string isSentStarter = orthographicHeuristic(token2);
				if (isSentStarter == "true")
				{
					token1.sentbreak = true;
					token1.abbr = true;
					continue;
				}
			
				if (token2.isFirstUpper() && params.sent_starters.find(nextType) != params.sent_starters.end())
				{
					token1.sentbreak = true;
					continue;
				}
			}


            // [4.3. Token-Based Detection of Initials and Ordinals]
            // Check if any initials or ordinals tokens that are marked
            // as sentbreaks should be reclassified as abbreviations.
            //
			if (isInitial || type == "##number##")
			{
				string isSentStarter = orthographicHeuristic(token2);
				if (isSentStarter == "false")
				{
					token1.sentbreak = false;
					token1.abbr = true;
					continue;
				}
			
				if (isSentStarter == "unknown" && isInitial && token2.isFirstUpper() && 
					!(params.ortho_context[nextType] & ORTHO_LC != 0))
				{
					token1.sentbreak = false;
					token1.abbr = true;
				}
			}
		}
	}

	string orthographicHeuristic(Token& token)
	{
		if(token.tok == ";" || token.tok == "," || token.tok == ":"
			|| token.tok == "." || token.tok == "!" || token.tok == "?")
			return "false";
		int orthoContext = params.ortho_context[token.typeNoSentperiod()];
		if (token.isFirstUpper() && (orthoContext & ORTHO_LC != 0) &&
			!(orthoContext & ORTHO_MID_UC != 0))
			return "true";
		if (token.isFirstLower() && ((orthoContext & ORTHO_UC != 0) || 
			!(orthoContext & ORTHO_BEG_LC != 0)))
			return "false";

		return "unknown";
	}
	
};

}
#endif
