#ifndef _SBD_PARAMS_H_
#define _SBD_PARAMS_H_

#include "common.h"
#include "collocation.h"

/**
 * parameters to be trained, which is used for the later
 * period classificaiton.
 * 
 * @author Eric - 2010.08.22
 *
 */
namespace sbd
{

struct Parameters
{

    // A set of word types for known abbreviations.
	set<string> abbrev_types;

    // A set of word type tuples for known common collocations
    // where the first word ends in a period. E.g., ('S.', 'Bach')
    // is a common collocation in a text that discusses 'Johann
    // S. Bach'. These count as negative evidence for sentence
    // boundaries.
	set<Collocation> collocations;

    // A set of word types for words that often appear at the
    // beginning of sentences.
	set<string> sent_starters;

    // A dictionary mapping word types to the set of orthographic
    // contexts that word type appears in. Contexts are represented
    // by adding orthographic context flags:
	OrthMap ortho_context;


	Parameters()
	{
		clearAbbrevs();
		clearCollocations();
		clearSentStarters();
		clearOrthoContext();
	}

	void clearAbbrevs()
	{
		abbrev_types.clear();
	}

	void clearCollocations()
	{
		collocations.clear();
	}

	void clearSentStarters()
	{
		sent_starters.clear();
	}

	void clearOrthoContext()
	{
		ortho_context.clear();
	}

	void addOrthoContext(string type, int flag)
	{
		ortho_context[type] |= flag;
	}
};


}
#endif
