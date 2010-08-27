#include <sbd/common.h>

using namespace sbd;

int main()
{
    Fdist mydict;
	string str = "hello";
    insertFdist(mydict, str);
    cout << "size:" << mydict.size() << endl;
	int a = getFdist(mydict, "hello");
	cout << a << endl;
	int b = getFdist(mydict, "b");
	cout << b << endl;
	cout << "size: " << mydict.size() << endl;

}

