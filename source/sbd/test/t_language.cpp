#include <ilplib.hpp>


using namespace sbd;

int main()
{
	string text = "Browning-Ferris closed yesterday at $28.50, up 37.5 cents, in New York Stock Exchange composite trading.";

	vector<Token> tokens;
	Language lang;

	lang.tokenize(text, tokens);
	for (int i=0; i<tokens.size(); ++i)
	{
		cout << tokens[i].tok << endl;
	}

}
