#include <iostream>
#include <boost/regex.hpp>

using namespace std;

int main()
{
	boost::regex reg("[^\\W\\d]");
	string text = "abc";
	boost::smatch what;
	if(boost::regex_search(text, what, reg))
	{
		cout << what.size() << endl;
		cout << what[0] << endl;
	}
}


