#include <iostream>
#include <set>
#include <ilplib.hpp>

using namespace std;
using namespace sbd;

int main()
{
	Collocation coll2("b", "c");
	Collocation coll1("a","b");


	set<Collocation> cols;
	cols.insert(coll1);
	cols.insert(coll2);

	for(set<Collocation>::const_iterator it = cols.begin();
			it != cols.end(); ++it)
	{
		Collocation col = (Collocation)*it;
		cout << col.token1 << ", " << col.token2 << endl;
	}
}
