#include <ilplib.hpp>

using namespace sbd;

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cout << "usage: t_token <token>" << endl;
		return -1;
	}
	Token token(argv[1]);
	cout << "tok: " << token.tok << endl;
	cout << "type: " << token.type << endl;
	cout << "period_final: " << token.period_final << endl;
	cout << "typeNoPeriod: " << token.typeNoPeriod() << endl;
	cout << "typeNoSentPeriod: " << token.typeNoSentperiod() << endl;
	cout << "firstUpper: " << token.isFirstUpper() << endl;
	cout << "firstLower: " << token.isFirstLower() << endl;
	cout << "firstCase: " << token.firstCase() << endl;
	cout << "isEllipsis: " << token.isEllipsis() << endl;
	cout << "isNumber: " << token.isNumber() << endl;
	cout << "isInitial: " << token.isInitial() << endl;
	cout << "isAlpha: " << token.isAlpha() << endl;
	cout << "isNonPunct: " << token.isNonPunct() << endl;


}
