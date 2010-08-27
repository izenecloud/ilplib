#include <sbd/common.h>

namespace sbd
{

int ORTHO_BEG_UC	= 1 << 1;
int ORTHO_MID_UC	= 1 << 2;
int ORTHO_UNK_UC	= 1 << 3;
int ORTHO_BEG_LC	= 1 << 4;
int ORTHO_MID_LC	= 1 << 5;
int ORTHO_UNK_LC	= 1 << 6;

int ORTHO_UC = ORTHO_BEG_UC + ORTHO_MID_UC + ORTHO_UNK_UC;
int ORTHO_LC = ORTHO_BEG_LC + ORTHO_MID_LC + ORTHO_UNK_LC;


int getOrthConst(string pos, string wordcase)
{
	if (pos == INITIAL  && wordcase == UPPER)
	{
		return ORTHO_BEG_UC;
	}
	else
	if (pos == INTERNAL && wordcase == UPPER)
	{
		return ORTHO_MID_UC;
	}
	else
	if (pos == UNKNOWN  && wordcase == UPPER)
	{
		return ORTHO_UNK_UC;
	}
	else
	if (pos == INITIAL  && wordcase == LOWER)
	{
		return ORTHO_BEG_LC;
	}
	else
	if (pos == INTERNAL && wordcase == LOWER)
	{
		return ORTHO_MID_LC;
	}
	else
	if (pos == UNKNOWN  && wordcase == LOWER)
	{
		return ORTHO_UNK_LC;
	}
    return ORTHO_UNK_LC;
}


////////////////////////////////////////////////////
// frequency distribution functions
////////////////////////////////////////////////////



void insertFdist(Fdist& fdist, string str)
{
	if (fdist.find(str) == fdist.end())
		fdist[str] = 1;
	else
		fdist[str] += 1;
}

int getFdist(Fdist& fdist, string str)
{
	if(fdist.find(str) == fdist.end())
		return 0;
	return fdist[str];		
}


void insertCollocationFdist(CollocationFdist& fdist, 
        Collocation& coll)
{
	if (fdist.find(coll) == fdist.end())
		fdist[coll] = 1;
	else
		fdist[coll] += 1;
}

}
