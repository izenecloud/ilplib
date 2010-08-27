#include "t_common.h"
#include <ilplib.hpp>


using namespace sbd;




int main()
{
    string text = "minute is a unit of measurement of time or of angle.   The minute is a unit of time equal to 1/60th of an hour or 60 seconds by 1. In the UTC time scale, a minute occasionally has 59 or 61 seconds; see leap second. The minute is not an SI unit; however, it is accepted for use with SI units. The symbol for minute or minutes is min. The fact that an hour contains 60 minutes is probably due to influences from the Babylonians, who used a base-60 or sexagesimal counting system. Colloquially, a min. may also refer to an indefinite amount of time substantially longer than the standardized length.";
	SBDTrainer trainer;

	vector<Token> tokens;
	trainer.tokenize(text, tokens);
	trainer.train(tokens);
	int abbrev_size =  trainer.params.abbrev_types.size();
	for (set<string>::const_iterator it = trainer.params.abbrev_types.begin();
		it!=trainer.params.abbrev_types.end(); ++it)
		cout << *it << endl;
		
	
	print(tokens);

}
