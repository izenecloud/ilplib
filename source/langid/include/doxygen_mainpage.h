/** \file doxygen_mainpage.h
 * Document for doxygen mainpage.
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date Feb 8, 2010
 */

/*! \mainpage iLI Library
<b>iLI</b> (iZENEsoft Language Identification) is a platform-independent C++ library to identify encoding and language.

<H2>Table of guidance</H2>
    <ul>
      <li><a href="#build">How to build and link with the library</a></li>
      <li><a href="#demo">How to run the demo</a></li>
      <li><a href="#model">How to build the models</a></li>
      <li><a href="#interface">How to use the interface</a></li>
        <ul>
          <li><a href="#interface_string">How to get string representation of identification result</a></li>
          <li><a href="#interface_sentence">How to tokenize sentence</a></li>
          <li><a href="#interface_region">How to print out language region</a></li>
          <li><a href="#interface_configure">How to configure the identification process</a></li>
            <ul>
              <li><a href="#interface_configure_limit">OPTION_TYPE_LIMIT_ANALYZE_SIZE</a></li>
              <li><a href="#interface_configure_block">OPTION_TYPE_BLOCK_SIZE_THRESHOLD</a></li>
              <li><a href="#interface_configure_chinese">OPTION_TYPE_NO_CHINESE_TRADITIONAL</a></li>
            </ul>
        </ul>
    </ul>

<H2><a name="build">How to build and link with the library</a></H2>

<a href="http://www.cmake.org">CMake</a> is used as the build system. The system could be built using script <CODE>build.sh</CODE> in directory <CODE>build</CODE> like below.
\code
$ cd build
$ ./build.sh
\endcode

After the project is built, the library targets <CODE>liblangid.a</CODE> is created in directory <CODE>lib</CODE>, and the executables in directory <CODE>bin</CODE> are created for demo and test.

To link with the library, the user application needs to include header files in directory <CODE>include</CODE>, and link the library files <CODE>liblangid.a</CODE> in directory <CODE>lib</CODE>.

An example of compiling user application <CODE>test.cpp</CODE> looks like:
\code
$ export LANGID_PATH=path_of_langid_project
$ g++ -I$LANGID_PATH/include -o test test.cpp $LANGID_PATH/lib/liblangid.a
\endcode

<H2><a name="demo">How to run the demo</a></H2>

To run the demo <CODE>bin/test_langid_run</CODE>, please make sure the project is built (see <a href="#build">how to build</a>).

Below is the demo usage:
\code
$ cd bin
$ ./test_langid_run -t encoding [-f INPUT_FILE]
$ ./test_langid_run -t language [-f INPUT_FILE]
$ ./test_langid_run -t list [-f INPUT_FILE]
$ ./test_langid_run -t segment [-f INPUT_FILE]
$ ./test_langid_run -t sentence [-f INPUT_FILE]
\endcode

In above demo usage, the <CODE>INPUT_FILE</CODE> could be set as an input file to analyze. If this option <CODE>-f</CODE> is not set, it would analyze each line from standard input and print its result. Below describes each demo usage in detail.

Demo of identifying character encoding of an input file.
\code
$ ./test_langid_run -t encoding -f INPUT_FILE
\endcode

Demo of identifying single primary language from standard input in UTF-8 encoding.
\code
$ ./test_langid_run -t language
\endcode

Demo of identifying a list of multiple languages of an input file in UTF-8 encoding.
\code
$ ./test_langid_run -t list -f INPUT_FILE
\endcode

Demo of segmenting a multi-lingual input file in UTF-8 encoding into single-language regions.
\code
$ ./test_langid_run -t segment -f INPUT_FILE
\endcode

Demo of sentence tokenization for an input file in UTF-8 encoding.
\code
$ ./test_langid_run -t sentence -f INPUT_FILE
\endcode

The demo source code could be available at: <a href="test__langid__run_8cpp-source.html">test_langid_run.cpp</a>. \n

<H2><a name="model">How to build the models</H2>
In below <CODE>Knowledge</CODE> interface, it is necessary to load the encoding model and language model in binary format.

\code
virtual bool loadEncodingModel(const char* fileName) = 0;
virtual bool loadLanguageModel(const char* fileName) = 0;
\endcode

These two models have been built in directory <CODE>db/model/</CODE> beforehand. In the case of supporting other platforms, you might need to build the binary models by yourself. Then you could run the script <CODE>langid\_build\_model.sh</CODE> in directory <CODE>bin</CODE> like below.

\code
$ cd bin
$ ./build_model.sh
\endcode

Now the model files <CODE>encoding.bin</CODE> and <CODE>language.bin</CODE> are created in directory <CODE>bin</CODE>, then you could load them using above <CODE>Knowledge</CODE> interface.

<H2><a name="interface">How to use the interface</a></H2>
<i>Step 1: Include the header files in directory <CODE>include</CODE></i>
\code
#include "langid/language_id.h"
#include "langid/factory.h"
#include "langid/knowledge.h"
#include "langid/analyzer.h"
\endcode

<i>Step 2: Use the library name space</i>
\code
using namespace langid;
\endcode

<i>Step 3: Call the interface and handle the result</i>

In the example below, the return value of some functions are not handled for simplicity. In your using, please properly handle those return values in case of failure.

The example code could be available at: <a href="test__langid__run_8cpp-source.html">test_langid_run.cpp</a>.

\code
// create instances
Factory* factory = Factory::instance();
Analyzer* analyzer = factory->createAnalyzer();
Knowledge* knowledge = factory->createKnowledge();

// load encoding model for encoding identification
knowledge->loadEncodingModel("db/model/encoding.bin");

// load language model for language identification or sentence tokenization
knowledge->loadLanguageModel("db/model/language.bin");

// set knowledge
analyzer->setKnowledge(knowledge);

// identify character encoding of string
EncodingID encID;
analyzer->encodingFromString("...", encID);

// identify character encoding of file
analyzer->encodingFromFile("...", encID);

// identify the single primary language of string in UTF-8 encoding
LanguageID langID;
analyzer->languageFromString("...", langID);

// identify the single primary language of file in UTF-8 encoding
analyzer->languageFromFile("...", langID);

// identify the list of multiple languages of string in UTF-8 encoding
vector<LanguageID> langIDVec;
analyzer->languageListFromString("...", langIDVec);

// identify the list of multiple languages of file in UTF-8 encoding
analyzer->languageListFromFile("...", langIDVec);

// segment the UTF-8 multi-lingual string into single-language regions
vector<LanguageRegion> regionVec;
analyzer->segmentString("...", regionVec);

// segment the UTF-8 multi-lingual document into single-language regions
analyzer->segmentFile("...", regionVec);

// get the length of the first sentence of string in UTF-8 encoding
int len = analyzer->sentenceLength("...");

delete knowledge;
delete analyzer;
\endcode

Below are some guidelines which might be helpful in your using the interface.
<H4><a name="interface_string">How to get string representation of identification result</a></H4>
When you have got the identification result (<CODE>EncodingID</CODE> or <CODE>LanguageID</CODE>), you might need to get their string representation. It could be achieved using below <CODE>Knowledge</CODE> interface.

\code
static const char* getEncodingNameFromID(EncodingID id);
static const char* getLanguageNameFromID(LanguageID id);
\endcode

<H4><a name="interface_sentence">How to tokenize sentence</a></H4>
Given a string in UTF-8 encoding, you could use <CODE>Analyzer::sentenceLength()</CODE> to perform sentence tokenization like below.

\code
const char* p = "...";
string sentStr;
while(int len = analyzer->sentenceLength(p))
{
    sentStr.assign(p, len);  // get each sentence
    cout << sentStr << endl; // print each sentence
    p += len;                // move to the begining of next sentence
}
\endcode

<H4><a name="interface_region">How to print out language region</a></H4>
When you have got <CODE>LanguageRegion</CODE> using <CODE>Analyzer::segmentString()</CODE>, you might need to print out the <CODE>i-th</CODE> region content like below.

\code
const char* str = "...";
vector<LanguageRegion> regionVec;
analyzer->segmentString(str, regionVec);

string regionStr(str + regionVec[i].start_, regionVec[i].length_);
cout << regionStr << endl;
\endcode

Similarly, when you have got <CODE>LanguageRegion</CODE> using <CODE>Analyzer::segmentFile()</CODE>, you might need to print out the <CODE>i-th</CODE> region content like below.

\code
const char* fileName = "...";
vector<LanguageRegion> regionVec;
analyzer->segmentFile(fileName, regionVec);

ifstream ifs(fileName);
ifs.seekg(regionVec[i].start_);

const unsigned int BUFFER_SIZE = 1024;
char buffer[BUFFER_SIZE];
unsigned int t;
for(unsigned int len = regionVec[i].length_; len; len-=t)
{
    t = min(len, BUFFER_SIZE);

    ifs.read(buffer, t);
    cout.write(buffer, t);
}
\endcode

<H4><a name="interface_configure">How to configure the identification process</a></H4>
Among the interface, <CODE>Analyzer::setOption(OptionType nOption, int nValue)</CODE> could be used to do some configurations in identification process. <CODE>OptionType</CODE> is an enumeration type defined below.

\code
enum OptionType
{
    OPTION_TYPE_LIMIT_ANALYZE_SIZE,
    OPTION_TYPE_BLOCK_SIZE_THRESHOLD,
    OPTION_TYPE_NO_CHINESE_TRADITIONAL,
    OPTION_TYPE_NUM
};
\endcode

And you could set value to each option like below.
\code
analyzer->setOption(Analyzer::OPTION_TYPE_LIMIT_ANALYZE_SIZE, 512);
analyzer->setOption(Analyzer::OPTION_TYPE_BLOCK_SIZE_THRESHOLD, 100);
analyzer->setOption(Analyzer::OPTION_TYPE_NO_CHINESE_TRADITIONAL, 1);
\endcode

The meaning of each option is described below.

<H4><a name="interface_configure_limit">OPTION_TYPE_LIMIT_ANALYZE_SIZE</a></H4>
This option is used to limit input size. The default value is 1024.

If a non-zero value is configured, it is used as the maximum input size for below <CODE>Analyzer</CODE> interfaces. That is, only the input bytes within this range would be used in these interfaces, and the rest bytes are just ignored. When the input file size is large, the upper bound of time consumption could be ensured by using this option.

\code
virtual bool encodingFromString(const char* str, EncodingID& id) = 0;
virtual bool encodingFromFile(const char* fileName, EncodingID& id) = 0;
virtual bool languageFromString(const char* str, LanguageID& id) = 0;
virtual bool languageFromFile(const char* fileName, LanguageID& id) = 0;
\endcode

If a zero value is configured, it uses all the input bytes in these interfaces, so that the highest accuracy could be achieved.

<H4><a name="interface_configure_block">OPTION_TYPE_BLOCK_SIZE_THRESHOLD</a></H4>
This option configures the threshold of language block size. The default value is zero.

If a non-zero value is configured, for below <CODE>Analyzer</CODE> interfaces, the language block (<CODE>LanguageRegion</CODE>), which size is not larger than this value, would be combined into adjacent larger block in a different language.

\code
virtual bool segmentString(const char* str, std::vector<LanguageRegion>& regionVec) = 0;
virtual bool segmentFile(const char* fileName, std::vector<LanguageRegion>& regionVec) = 0;
\endcode

If a zero value is configured, it disables combining blocks in different languages.

<H4><a name="interface_configure_chinese">OPTION_TYPE_NO_CHINESE_TRADITIONAL</a></H4>
This option configures not to use Chinese Traditional language. The default value is zero.

If a non-zero value is configured, for below <CODE>Analyzer</CODE> interfaces, Chinese Traditional text would be identified as Chinese Simplified language.

\code
virtual bool languageFromString(const char* str, LanguageID& id) = 0;
virtual bool languageFromFile(const char* fileName, LanguageID& id) = 0;
virtual bool languageListFromString(const char* str, std::vector<LanguageID>& idVec) = 0;
virtual bool languageListFromFile(const char* fileName, std::vector<LanguageID>& idVec) = 0;
virtual bool segmentString(const char* str, std::vector<LanguageRegion>& regionVec) = 0;
virtual bool segmentFile(const char* fileName, std::vector<LanguageRegion>& regionVec) = 0;
\endcode

If a zero value is configured, Chinese Traditional text would be identified as Chinese Traditional language as original.

\n
Copyright (C) 2010, iZENEsoft. All rights reserved. \n
Author: Jun <jun.zju@gmail.com> \n
Version: 1.0.0 \n
Date: 2010-02-08 \n
*/
