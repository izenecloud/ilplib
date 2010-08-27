#include "t_common.h"
#include <ilplib.hpp>

using namespace sbd;

void loadString(string& text, string& filename)
{
	fstream file(filename.c_str());
    char ch;
	while(file.good())
	{
		file.get(ch);
        if(file.good())
     		text.push_back(ch);
	}	
	file.close();

}

int main(int argc, char** argv)
{

	if(argc < 2) 
	{
		cout << "t_sbd <file>" << endl;
		return -1;
	}
	string text;
	string filename = argv[1];
	loadString(text, filename);

	SBD sbd;
	sbd.train(text);
	vector<string> sentences;
	sbd.extractSentences(text, sentences);

	print(sentences);

    

}
