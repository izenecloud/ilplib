#include <question-answering/QuestionAnalysis.h>
#include <fstream>

using namespace std;

namespace ilplib{ namespace qa{

boost::unordered_set<std::string> QuestionAnalysis::questionWordList_;

boost::unordered_set<std::string> QuestionAnalysis::candidates_;

std::string QuestionAnalysis::resourcePath_;

QuestionAnalysis::QuestionAnalysis()
{
    candidates_.insert("A");
    candidates_.insert("AD");
    candidates_.insert("C");
    candidates_.insert("M");
    candidates_.insert("J");
    candidates_.insert("MQ");
    candidates_.insert("NG");
    candidates_.insert("N");
    candidates_.insert("NR");
    candidates_.insert("NS");
    candidates_.insert("NT");
    candidates_.insert("NX");
    candidates_.insert("NZ");
    candidates_.insert("P");
    candidates_.insert("Q");
    candidates_.insert("R");
    candidates_.insert("TG");
    candidates_.insert("T");
    candidates_.insert("VD");
    candidates_.insert("VG");
    candidates_.insert("VN");
    candidates_.insert("V");
}

QuestionAnalysis::~QuestionAnalysis()
{
}

void QuestionAnalysis::load(const std::string& path)
{
    resourcePath_ = path;
    static boost::once_flag once = BOOST_ONCE_INIT;
    boost::call_once(&QuestionAnalysis::loadQuestionDict, once );
}

void QuestionAnalysis::loadQuestionDict()
{
    ifstream ifs(resourcePath_.c_str());
    string line;
    boost::unordered_set<std::string>::iterator iter;
    while( getline(ifs, line) ){
	boost::trim(line);
	iter = questionWordList_.find(line);
	if(iter == questionWordList_.end())
           questionWordList_.insert(line);
    }
    ifs.close();
}

bool QuestionAnalysis::isQuestion(const std::string& question)
{
    boost::unordered_set<std::string>::iterator iter;
    for(iter = questionWordList_.begin(); iter != questionWordList_.end(); ++iter)
    {
        if(question.find(*iter)!=std::string::npos)
			return true;
    }
    return false;
}

}}
