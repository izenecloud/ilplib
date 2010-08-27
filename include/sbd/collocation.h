#ifndef _SBD_COLLOCATION_H_
#define _SBD_COLLOCATION_H_

namespace sbd
{

struct Collocation
{
	std::string token1;
	std::string token2;

	Collocation(std::string tok1, std::string tok2):
            token1(tok1), token2(tok2){}

	bool operator<(const Collocation& src) const
	{
		if (token1 != src.token1)
		{
			return token1 < src.token1;
		}
		else
		{
			return token2 < src.token2;
		}
	}

	bool operator==(const Collocation& src) const
	{
		return token1 == src.token1 && 
                token2 == src.token2;
	}

	bool operator!=(const Collocation& src) const
	{
		return !operator==(src);
	}
};

}

#endif
