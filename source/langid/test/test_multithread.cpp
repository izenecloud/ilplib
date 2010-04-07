/** \file test_multithread.cpp
 * Test langid library in multithread environment.
 * Below is the usage examples:
 * \code
 * identify primary language of each line in inputFile.utf8, and print the result to outputFile
 * $./test_multithread inputFile.utf8 outputFile
 *
 * \endcode
 * 
 * \author Jun Jiang
 * \version 0.1
 * \date 01 12, 2010
 */

#include "langid/language_id.h"
#include "langid/factory.h"
#include "langid/knowledge.h"
#include "langid/analyzer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cassert>

#include <sstream>
#include <pthread.h>

using namespace std;
using namespace ilplib::langid;

/**
 * Data for each thread.
 */
struct pthread_t_data
{
    unsigned int thread_id;
    string input_filename;
    string output_filename;
    Analyzer* analyzer;
};

void* analyzerWithThread(void* threadarg)
{
    pthread_t_data* data;
    data = (struct pthread_t_data*)threadarg;
    Analyzer* _analyzer = data->analyzer;

    ostringstream ss;
    ss << data->thread_id;
    string source = data->input_filename;
    string dest = data->output_filename +  ".output-" + ss.str();

    ifstream ifs(source.c_str());
    if(! ifs)
    {
        cerr << "Thread No." << data->thread_id << "=> error in opening file " << source << endl;
        pthread_exit(NULL);
    }

    ofstream ofs(dest.c_str());
    if(! ofs)
    {
        cerr << "Thread No." << data->thread_id << "=> error in creating file " << dest << endl;
        pthread_exit(NULL);
    }

    string line;
    LanguageID lineID;
    while(getline(ifs, line))
    {
        if(line.empty())
            continue;

        if(! _analyzer->languageFromString(line.c_str(), lineID))
        {
            cerr << "Thread No." << data->thread_id << "=> error in analyze string " << line << endl;
            pthread_exit(NULL);
        }

        ofs << Knowledge::getLanguageNameFromID(lineID) << endl;
    }

    cerr << "Thread No." << data->thread_id << "=> succeed in languageFromString() from " << source << " to " << dest << endl;

    pthread_exit(NULL);
}

/**
 * Main function.
 */
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cerr << "Usage: " << argv[0] << " input.utf8 output.utf8" << endl;
        exit(1);
    }

    // create instances
    Factory* factory = Factory::instance();
    Analyzer* analyzer = factory->createAnalyzer();
    Knowledge* knowledge = factory->createKnowledge();

    // model files
    const char* encodingModel = "../db/langid/model/encoding.bin";
    const char* languageModel = "../db/langid/model/language.bin";

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

    const char* inputFile = argv[1];
    const char* outputFile = argv[2];

    // multi-thread test
    int rc;
    unsigned int tid = 0;

    const unsigned int NUM_THREADS = 100;
    pthread_t threads[NUM_THREADS];
    pthread_t_data thread_data[NUM_THREADS];

    while(tid < NUM_THREADS)
    {
        thread_data[tid].thread_id = tid;
        thread_data[tid].input_filename = inputFile;
        thread_data[tid].output_filename = outputFile;

        thread_data[tid].analyzer = analyzer;

        rc = pthread_create(&threads[tid], NULL, analyzerWithThread, (void *) &thread_data[tid]);
        if(rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }

        ++tid;
    }

    for(unsigned int i=0; i<NUM_THREADS; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    delete knowledge;
    delete analyzer;

    return 0;
}
