#ifndef _SBD_TRAINER_H_
#define _SBD_TRAINER_H_

#include "common.h"
#include "token.h"
#include "collocation.h"
#include "sbd_base.h"
#include <boost/tuple/tuple.hpp> 
#include <boost/tuple/tuple_io.hpp> 

/**
 * sbd trainer.
 *
 * @author Eric - 2010.08.22
 */
namespace sbd
{

struct AbbrevPredictRes
{
	string abbr;
	float score;
	bool isAdd;
};


class SBDTrainer : public SBDBase
{

private:

	static const float		ABBREV = 0.3;
	static const bool		IGNORE_ABBREV_PENALTY = false;
	static const int		ABBREV_BACKOFF = 5;
	static const float		COLLOCATION = 7.88;
	static const int		SENT_STARTER = 30;
	static const bool		INCLUDE_ALL_COLLOCS = true;
	static const bool		INCLUDE_ABBREV_COLLOCS = true;
	static const int		MIN_COLLOC_FREQ = 1;

    int period_count;
	int sentence_break_count;
	int word_count;

    // A frequency distribution giving the frequency of each
    // case-normalized token type in the training data.
	Fdist type_fdist;

    // A frequency distribution giving the frequency of all
    // bigrams in the training data where the first word ends in a
    // period.
	CollocationFdist collocation_fdist;

    // A frequency distribution giving the frequency of all words
    // that occur at the training data at the beginning of a sentence
    // (after the first pass of annotation).
	Fdist sentence_starter_fdist;

	bool finalized;


public:

    SBDTrainer(): period_count(0), sentence_break_count(0), word_count(0), finalized(false)
    {}


	Parameters getParams()
	{
		if(!finalized)
		{
			finalize();
		}
		return params;
	}

	void finalize()
	{
		params.clearSentStarters();
		findSentenceStarters();
		params.clearCollocations();
		findCollocations();
		finalized = true;
	}

	void train(string& text)
	{
		vector<Token> tokens;
		tokenize(text, tokens);
		train(tokens);
	}

	void train(vector<Token>& tokens)
	{
		set<string> typeSet;

		for (size_t i=0; i<tokens.size(); ++i)
		{
			Token token = tokens[i];
			typeSet.insert(token.type);
			insertFdist(type_fdist, token.type);
			word_count++;
			if (token.period_final)
			{
				period_count++;
			}
		}

		// find abbreviation
		vector<AbbrevPredictRes> results;
		reclassifyAbbrevTypes(typeSet, results);
		for (size_t i=0; i<results.size(); ++i) 
		{
		string type = results[i].abbr;
		float score = results[i].score;
		bool isAdd = results[i].isAdd;
		//cout << type << ", " << score << ", " << isAdd << endl;
		if (score >= ABBREV)
		{
			if (isAdd) {
#ifdef SBD_DEBUG
				cout << "Add Abbrev: [" << score << "] " << type << endl;
#endif
				params.abbrev_types.insert(type);
			}
		} else
		{
			if (!isAdd) {
#ifdef SBD_DEBUG
				cout << "Remove Abbrev: " << type << endl;
#endif
				params.abbrev_types.erase(type);
			}
		}
		}
		// make a preliminary pass through document, marking likely
		// sentence breaks, abbves, and ellipsis.
		annotateFirstPass(tokens);

        // check what contexts each word type can appear in, given the
        // case of its first letter.
		getOrthographyData(tokens);

		for (size_t i=0; i<tokens.size(); ++i)
		{
			if (tokens[i].sentbreak)
			{
				sentence_break_count++;
			}
		}
		//
		for (size_t i=0; i<tokens.size()-1; ++i)
		{
			Token token1 = tokens[i];
			Token token2 = tokens[i+1];
			if (!token1.period_final)
				continue;

			// is the first token a rare abbrev.
			if (isRareAbbrevType(token1, token2))
			{
#ifdef SBD_DEBUG
				cout << "Rare Abbrev: " << token1.type << endl;
#endif
				params.abbrev_types.insert(token1.typeNoPeriod());
			}
			// if the second token has a high likelihood of starting a sentence
			if (isPotentialSentStarter(token1, token2))
			{
				insertFdist(sentence_starter_fdist, token2.type);
			}

			// if the bigram is a collocation
			if (isPotentialCollocation(token1, token2))
			{
				//cout << "Potential Coll: " << token1.type << ", " << token2.type << endl;
				Collocation collocation(token1.typeNoPeriod(), token2.typeNoPeriod());
				insertCollocationFdist(collocation_fdist, collocation);
			}
		}

	}

private:

	///////////////////////////////////////////////////////////////////
	// Log Likelihoods
	///////////////////////////////////////////////////////////////////

	/*
	  A function that calculates the modified Dunning log-likelihood
	  ratio scores for abbreviation candidates. The details of how
	  this works is available in the paper.
	 */
	float modifiedLogLikelihood(int count_a, int count_b, int count_ab, int N)
	{
		float p1 = 1.0 * count_b / N;
		float p2 = 0.99;

		float null_hypo = count_ab * log(p1) + (count_a - count_ab) * log(1.0 - p1);
		float alt_hypo  = count_ab * log(p2) + (count_a - count_ab) * log(1.0 - p2);

		float likelihood = null_hypo - alt_hypo;

		return (-2.0 * likelihood);
	}

	/*
	  A function that will just compute log-likelihood estimate, in
	  the original paper it's decribed in algorithm 6 and 7.

	  This *should* be the original Dunning log-likelihood values,
	  unlike the previous log_l function where it used modified
	  Dunning log-likelihood values
	 */
	float logLikelihood(int count_a, int count_b, int count_ab, int N)
	{
		float p = 1.0 * count_b / N;
		float p1 = 1.0 * count_ab / count_a;
		float p2 = 1.0 * (count_b - count_ab) / (N - count_a);

		float summand1 = count_ab * log(p) +
						   (count_a - count_ab) * log(1.0 - p);
		float summand2 = (count_b - count_ab) * log(p) +
						   (N - count_a - count_b + count_ab) * log(1.0 - p);

		float summand3 = 0;
		float summand4 = 0;

		if (count_a != count_ab)
		{
			summand3 = count_ab * log(p1) +
					   (count_a - count_ab) * log(1.0 - p1);
		}

		if (count_b != count_ab)
		{
			summand4 = (count_b - count_ab) * log(p2) +
					   (N - count_a - count_b + count_ab) * log(1.0 - p2);
		}

		float likelihood = summand1 + summand2 - summand3 - summand4;

		return (-2.0 * likelihood);
	}

	///////////////////////////////////////////////////////////////////
	// Sentence starter
	///////////////////////////////////////////////////////////////////

	bool isPotentialSentStarter(Token& token1, Token& token2)
	{
		return (token1.sentbreak &&
				(!token1.isNumber() || token1.isInitial()) &&
				token2.isAlpha());
	}

	void findSentenceStarters()
	{
		for (Fdist::const_iterator it = sentence_starter_fdist.begin();
			it != sentence_starter_fdist.end(); ++it)
		{
			string type = it->first;
			int breakCount = it->second;

			if (type.empty()) continue;
			int typeCount = getFdist(type_fdist, type) + getFdist(type_fdist, type + ".");
			if (typeCount < breakCount) continue;
			
			float ll = logLikelihood(sentence_break_count,
				typeCount, breakCount, word_count);

			if (ll >= SENT_STARTER && 
				1.0*word_count/sentence_break_count > 1.0*typeCount/breakCount)
				params.sent_starters.insert(type);
		}
	}


	///////////////////////////////////////////////////////////////////
	// Collocations
	///////////////////////////////////////////////////////////////////

	bool isPotentialCollocation(Token& token1, Token& token2)
	{

		//cout << token1.type << ", " << token2.type << endl;
		//cout << token1.isNonPunct() << endl;
		//cout << token2.isNonPunct() << endl;
		if( (INCLUDE_ALL_COLLOCS || 
		    ( INCLUDE_ABBREV_COLLOCS && token1.abbr) ||
			  (
			    token1.sentbreak &&
			    (
			     token1.isNumber() or
			     token2.isInitial()
			     )
			   )
			 )
		   	 && token1.isNonPunct()
		   	 && token2.isNonPunct()
		   )
		{
			return true;
		}

		return false;

	}

	void findCollocations()
	{
		//cout << "*** find collocation...." << endl;
		for (CollocationFdist::const_iterator it = collocation_fdist.begin();
				it != collocation_fdist.end(); ++it)
		{
			Collocation coll = it->first;
			int collCount = it->second;

			string type1 = coll.token1;
			string type2 = coll.token2;

			if (type1.empty() || type2.empty())
				continue;
			if (params.sent_starters.find(type2) != params.sent_starters.end())
				continue;

			int type1Count = getFdist(type_fdist, type1) + getFdist(type_fdist, type1 + ".");
			int type2Count = getFdist(type_fdist, type2) + getFdist(type_fdist, type2 + ".");



			if (type1Count > 1 && type2Count > 1 && MIN_COLLOC_FREQ < collCount
					&& collCount <= min(type1Count, type2Count))
			{
				float ll = logLikelihood(type1Count, type2Count, collCount, word_count);
				if (ll >= COLLOCATION && (1.0 * word_count/type1Count) > (1.0 * type2Count/collCount))
				{
#ifdef SBD_DEBUG
					cout << "Collocation: [" << ll << "] " << type1 << ", " << type2 << endl;
#endif
					params.collocations.insert(Collocation(type1, type2));
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////
	// Orthographic Data
	///////////////////////////////////////////////////////////////////

    /*
     *  Collect information about whether each token type occurs
     *  with different case patterns (i) overall, (ii) at
     *  sentence-initial positions, and (iii) at sentence-internal
     *  positions
     */
	void getOrthographyData(vector<Token>& tokens)
	{
		string context = "internal";
		
		for (size_t i=0; i<tokens.size(); ++i)
		{
			Token token = tokens[i];
			if (token.parastart && context != "unknown") 
				context = "initial";
			if (token.linestart && context == "internal")
				context = "unknown";

			string type = token.typeNoSentperiod();
			int flag = getOrthConst(context, token.firstCase());
			if (flag)
				params.addOrthoContext(type, flag);

			if (token.sentbreak)
				context = !(token.isNumber() || token.isInitial())? "initial":"unknown";
			else if (token.ellipsis || token.abbr)
				context = "unknown";
			else
				context = "internal";
		}
	}


	///////////////////////////////////////////////////////////////////
	// Abbreviations
	///////////////////////////////////////////////////////////////////

	bool isRareAbbrevType(Token& token1, Token& token2)
	{
		if (token1.abbr || !token1.sentbreak)
			return false;

		string type = token1.typeNoSentperiod();
		int count = getFdist(type_fdist,type) + getFdist(type_fdist, type.substr(0, type.size()-1));
		if (params.abbrev_types.find(type) != params.abbrev_types.end() || count >= ABBREV_BACKOFF)
			return false;

		if (language.internal_punct.find(token2.tok[0]) != string::npos)
		{
			return true;
		}
		else if (token2.isFirstLower())
		{
			string type2 = token2.typeNoSentperiod();
			int type2OrthContext = params.ortho_context[type2];
			if ((type2OrthContext & ORTHO_BEG_UC) && (type2OrthContext & ORTHO_MID_UC))
			{
				return true;
			}
		}
		return false;

	}

    /*
     * (Re)classifies each given token if
     *   - it is period-final and not a known abbreviation; or
     *   - it is not period-final and is otherwise a known abbreviation
     *  by checking whether its previous classification still holds according
     *  to the heuristics of section 3.
     */
	void reclassifyAbbrevTypes(set<string>& typeSet, vector<AbbrevPredictRes>& results)
	{
		float score = 0;
		bool isAdd = false;

		for (set<string>::const_iterator it = typeSet.begin(); it != typeSet.end(); ++it)
		{
			string type = *it;
			boost::regex expr("[^\\W\\d]");
			boost::smatch what;
			if(!boost::regex_search(type, what, expr) || type == "##number##")
				continue;
			if(boost::algorithm::ends_with(type, "."))
			{
				if (params.abbrev_types.find(type) != params.abbrev_types.end())
					continue;
				type = type.substr(0, type.size()-1);
				isAdd = true;
			} else
			{
				if (params.abbrev_types.find(type) == params.abbrev_types.end())
					continue;
				isAdd = false;
			}

			int periodCount = 0;
			int nonPeriodCount = 0;

			for (size_t i=0; i<type.size(); ++i)
			{
				if (type[i] == '.')
				{
					periodCount++;
				} else {
					nonPeriodCount++;
				}
			}
			periodCount++;

			int withPeriodCount = getFdist(type_fdist, type+".");

			int withoutPeriodCount = getFdist(type_fdist, type);

//			cout << "wpc: " << withPeriodCount << endl;
//			cout << "wopc: " << withoutPeriodCount << endl;
//			cout << "pc: " << period_count << endl;
//			cout << "n: " << word_count << endl;

			float ll = modifiedLogLikelihood(withPeriodCount+withoutPeriodCount,
							period_count, withPeriodCount, word_count);
			float flength = exp(-nonPeriodCount);
			float fperiods = periodCount;
			float fpenalty = IGNORE_ABBREV_PENALTY ? 0 : pow( 1.0*nonPeriodCount, -withoutPeriodCount);

			score = ll * flength * fperiods * fpenalty;

//			cout << "ll: " << ll << endl;
//			cout << "flenth: " << flength << endl;
//                      cout << "fperiods: " << fperiods << endl;
//			cout << "fpenalty: " << fpenalty << endl;
//			cout << "score: " << score << endl;
//			cout << "-------------------------------" << endl;
			AbbrevPredictRes result;
			result.abbr = type;
			result.score = score;
			result.isAdd = isAdd;
			results.push_back(result);

		}
	}

};

}

#endif
