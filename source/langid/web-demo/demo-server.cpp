/** \file demo-server.cpp
 * A web server for langid demo.
 * Below is the usage examples:
 * \code
 * Start the web server:
 * $./demo-server
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Jan 26, 2010
 */

#include "langid/language_id.h"
#include "langid/factory.h"
#include "langid/knowledge.h"
#include "langid/analyzer.h"


#include "appws.h"

#include <string>
#include <sstream> //ostringstream
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstdio>

#define ALIAS_URI "/language-identification"
static const unsigned int AWS_MAX_BUFFER_SIZE = 8192;

using namespace std;
using namespace ilplib::langid;

/**
 * Wrapper class to print content to AWS stream.
 */
class AWS_Stream
{
public:
    AWS_Stream(aws_connect_t* connect)
        : connect_(connect)
    {
    }

    template<class T> AWS_Stream& operator<<(T any)
    {
        ostringstream ost;
        ost << any;
        aws_printf(connect_, ost.str().c_str());
        return *this;
    }

    bool writeBuffer(const char *buf, int len)
    {
        if(aws_write(connect_, buf, len) == len)
            return true;

        return false;
    }

    bool includeFile(const char* fileName)
    {
        assert(fileName);

        ifstream ifs(fileName);
        if(! ifs)
        {
            cerr << "error: file not found " << fileName << endl;
            return false;
        }

        string line;
        while(getline(ifs, line))
        {
            if(! ifs.eof())
                line += '\n';

            aws_printf(connect_, line.c_str());
        }

        return true;
    }

private:
    aws_connect_t* connect_;
};

template<> AWS_Stream& AWS_Stream::operator<<(const char* any)
{
    aws_printf(connect_, any);
    return *this;
}

/**
 * Print the language regions in string.
 * \param to AWS output stream
 * \param regionVec the language regions
 */
void printRegionList(AWS_Stream& to, const vector<LanguageRegion>& regionVec)
{
    vector<bool> isPrintVec(LANGUAGE_ID_NUM, false);

    to << "<table border=\"0\" cellpadding=\"15\" style=\"width:100%;font-size:15px\">";
    to << "<tr><th width=\"20%\" align=\"center\">language list</th>";
    to << "<th width=\"80%\" align=\"left\">language blocks</th></tr>";
    to << "<tr><td valign=\"top\" align=\"center\">";

    for(unsigned int i=0; i<regionVec.size(); ++i)
    {
        // ensure print only once
        if(! isPrintVec[regionVec[i].languageID_])
        {
            isPrintVec[regionVec[i].languageID_] = true;

            to << "<div class=\"languagebg" << regionVec[i].languageID_ << "\">";
            to << Knowledge::getLanguageNameFromID(regionVec[i].languageID_);
            to << "</div>";
        }
    }
    to << "</td><td valign=\"top\">";
}
    
/**
 * Print the language regions in string.
 * \param to AWS output stream
 * \param str the pointer to the start of string
 * \param regionVec the language regions
 */
void printStringRegion(AWS_Stream& to, const char* str, const vector<LanguageRegion>& regionVec)
{
    printRegionList(to, regionVec);
    
    for(unsigned int i=0; i<regionVec.size(); ++i)
    {
        to << "<span class=\"resultbg" << regionVec[i].languageID_ << "\" title=\"";
        to << "region " << i << ", start: " << regionVec[i].start_ << ", length: " << regionVec[i].length_ << ", language: " << Knowledge::getLanguageNameFromID(regionVec[i].languageID_);
        to << "\">";
        //if(regionVec[i].length_ < AWS_MAX_BUFFER_SIZE)
        //{
            //to << string(str + regionVec[i].start_, regionVec[i].length_);
        //}

        // AWS limit the print buffer size
        const unsigned int len = regionVec[i].length_;
	unsigned int pos = 0;
        while(pos < len)
        {
            unsigned int t = min(len-pos, AWS_MAX_BUFFER_SIZE);
            to.writeBuffer(str + regionVec[i].start_ + pos, t);
            pos += t;
        }

        to << "</span>";
    }

    to << "</td></tr></table>";
}

/**
 * Print the language regions in file.
 * \param to AWS output stream
 * \param fileName the file name
 * \param regionVec the language regions
 */
void printFileRegion(AWS_Stream& to, const char* fileName, const vector<LanguageRegion>& regionVec)
{
    printRegionList(to, regionVec);
    
    ifstream ifs(fileName);
    if(! ifs)
    {
        to << "</td></tr></table>";
        to << "error in opening file " << fileName << "\n";
        return;
    }

    for(unsigned int i=0; i<regionVec.size(); ++i)
    {
        LanguageID id = regionVec[i].languageID_;

        to << "<span class=\"resultbg" << id << "\" title=\"";
        to << "region " << i << ", start: " << regionVec[i].start_ << ", length: " << regionVec[i].length_ << ", language: " << Knowledge::getLanguageNameFromID(id);
        to << "\">";

        const unsigned int BUFFER_SIZE = 1024;
        char buffer[BUFFER_SIZE];
        unsigned int t;
        for(unsigned int len = regionVec[i].length_; len; len-=t)
        {
            t = min(len, BUFFER_SIZE);

            ifs.read(buffer, t);
            to.writeBuffer(buffer, t);
        }
        to << "</span>";

    }

    to << "</td></tr></table>";
}

/**
 * Print out encoding identification result.
 */
static void identifyEncoding(struct aws_connect_t *conn,
        const struct aws_request_t *request_info,
        void *user_data)
{
    AWS_Stream to(conn);

    Analyzer* analyzer = reinterpret_cast<Analyzer*>(user_data);
    if(! analyzer)
    {
        const char* error = "error: null pointer of analyzer.";
        cerr << error << endl;
        to << error;
        return;
    }

    to.includeFile("web/result_head.html");

    // identify encoding of uploaded file
    struct aws_file_t *x = aws_find_post_file(conn, "encodingFile");
    if(x)
    {
        string fileName("upload/");
        fileName += x->fn;
        string resultStr;
        if(! aws_save_as(x->data, x->data_len, fileName.c_str()))
        {
            to << "error: failed to save uploaded file " << fileName << "\n";
            to.includeFile("web/result_foot.html");
            aws_free(x);
            return;
        }

	EncodingID id;
	if(analyzer->encodingFromFile(fileName.c_str(), id))
	{
		resultStr = Knowledge::getEncodingNameFromID(id);
	}
	else
	{
		resultStr = "error to get encoding ID from file " + fileName;
		cerr << resultStr << endl;
	}
	aws_free(x);

        to << "<span class=\"resultbg\">" << resultStr << "</span>\n";
    }

    to.includeFile("web/result_foot.html");
}

/**
 * Print out language identification result.
 */
static void identifyLanguage(struct aws_connect_t *conn,
        const struct aws_request_t *request_info,
        void *user_data)
{
    AWS_Stream to(conn);

    Analyzer* analyzer = reinterpret_cast<Analyzer*>(user_data);
    if(! analyzer)
    {
        const char* error = "error: null pointer of analyzer.";
        cerr << error << endl;
        to << error;
        return;
    }

    to.includeFile("web/result_head.html");

    const char* inputMethod = aws_find_post_var(conn, "inputmethod");
    if(! inputMethod)
    {
        to << "error: no inputmethod in request\n";
        to.includeFile("web/result_foot.html");
        return;
    }

    int inputIndex = atoi(inputMethod);
    LanguageID id;
    string resultStr;
    bool analyzeResult = false;
    string fileName;
    const char* inputText = 0;

    // file uploaded
    if(inputIndex == 0)
    {
        struct aws_file_t *x = aws_find_post_file(conn, "languageFile");
        if(! x)
        {
            to << "error: no languageFile in request\n";
            to.includeFile("web/result_foot.html");
            return;
        }

        fileName = "upload/";
        fileName += x->fn;
        if(! aws_save_as(x->data, x->data_len, fileName.c_str()))
        {
            to << "error: failed to save uploaded file " << fileName << "\n";
            to.includeFile("web/result_foot.html");
            aws_free(x);
            return;
        }

        aws_free(x);
    }
    // text input
    else if(inputIndex == 1)
    {
        inputText = aws_find_post_var(conn, "languageText");
        if(! inputText)
        {
            to << "error: no languageText in request\n";
            to.includeFile("web/result_foot.html");
            return;
        }

    }
    else
    {
        to << "error: unknown inputIndex " << inputIndex << " in request, inputMethod: " << inputMethod << "\n";
        to.includeFile("web/result_foot.html");
        return;
    }

    const char* executeMethod = aws_find_post_var(conn, "executemethod");
    if(! executeMethod)
    {
        to << "error: no executemethod in request\n";
        to.includeFile("web/result_foot.html");
        return;
    }

    int executeIndex = atoi(executeMethod);
    // primary language
    if(executeIndex == 0)
    {
        if(inputIndex == 0)
        {
            analyzeResult = analyzer->languageFromFile(fileName.c_str(), id);
            if(! analyzeResult)
                resultStr = "error to get language ID from file " + fileName;
        }
        else if(inputIndex == 1)
        {
            analyzeResult = analyzer->languageFromString(inputText, id);
            if(! analyzeResult)
            {
                resultStr = "error to get language ID from string ";
                resultStr += inputText;
            }
        }

        // output result
        if(analyzeResult)
            resultStr = Knowledge::getLanguageNameFromID(id);

        to << "<span class=\"resultbg\">" << resultStr << "</span>\n";
    }
    // list multiple language
    else if(executeIndex == 1)
    {
        vector<LanguageID> idVec;
        if(inputIndex == 0)
        {
            analyzeResult = analyzer->languageListFromFile(fileName.c_str(), idVec);
            if(! analyzeResult)
                resultStr = "error to get multiple languages from file " + fileName;
        }
        else if(inputIndex == 1)
        {
            analyzeResult = analyzer->languageListFromString(inputText, idVec);
            if(! analyzeResult)
            {
                resultStr = "error to get multiple languages from string ";
                resultStr += inputText;
            }
        }

        // output result
        if(analyzeResult)
        {
            for(unsigned int i=0; i<idVec.size(); ++i)
            {
                resultStr = Knowledge::getLanguageNameFromID(idVec[i]);
                //to << "<span class=\"resultbg\">" << resultStr << "</span><br>\n";
                to << "<p><span class=\"resultbg\">" << resultStr << "</span></p>\n";
            }
        }
        else
            to << "<span class=\"resultbg\">" << resultStr << "</span>\n";
    }
    // segment language
    else if(executeIndex == 2)
    {
        vector<LanguageRegion> regionVec;
        if(inputIndex == 0)
        {
            if(analyzer->segmentFile(fileName.c_str(), regionVec))
            {
                printFileRegion(to, fileName.c_str(), regionVec);
            }
            else
            {
                resultStr = "error to segment file " + fileName;
                to << "<span class=\"resultbg\">" << resultStr << "</span>\n";
                to.includeFile("web/result_foot.html");
                return;
            }

        }
        else if(inputIndex == 1)
        {
            if(analyzer->segmentString(inputText, regionVec))
            {
                printStringRegion(to, inputText, regionVec);
            }
            else
            {
                resultStr = "error to segment languages from string ";
                resultStr += inputText;
                to << "<span class=\"resultbg\">" << resultStr << "</span>\n";
                to.includeFile("web/result_foot.html");
                return;
            }

        }
    }
    else
    {
        to << "error: unknown executeIndex " << executeIndex << " in request, executeMethod: " << executeMethod << "\n";
    }

    to.includeFile("web/result_foot.html");
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    // create langid instances
    Factory* factory = Factory::instance();
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // model files
    const char* encodingModel = "../db/model/encoding.bin";
    const char* languageModel = "../db/model/language.bin";

    // load encoding model for encoding identification
    if(! knowledge->loadEncodingModel(encodingModel))
    {
        cerr << "error: fail to load file " << encodingModel << endl;
        exit(1);
    }

    // load language model for language identification or sentence tokenization
    if(! knowledge->loadLanguageModel(languageModel))
    {
        cerr << "error: fail to load file " << languageModel << endl;
        exit(1);
    }

    // set knowledge
    analyzer->setKnowledge(knowledge);
    
    struct aws_context_t *ctx;

    ctx = aws_start();
    aws_set_option(ctx, "error_log", "demo-server_error.log");
    aws_set_option(ctx, "aliases", ALIAS_URI "=./web");
    aws_set_option(ctx, "ports", "8082");
    aws_set_uri_callback(ctx, ALIAS_URI "/identify_encoding.html", &identifyEncoding, analyzer);
    aws_set_uri_callback(ctx, ALIAS_URI "/identify_language.html", &identifyLanguage, analyzer);

    cout << "Please visit page: http://127.0.0.1:8082" << ALIAS_URI << "/index.html" << endl;

    (void) getchar();
    cout << "Going to quit... ";
    aws_stop(ctx);	
    cout << "done." << endl;
    return 0;
}
