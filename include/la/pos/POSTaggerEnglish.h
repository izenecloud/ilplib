/*
 * POSTaggerEnglish.h
 *
 *  Created on: 2009-6-30
 *      Author: zhjay
 */

#ifndef _LA_POSTAGGERENGLISH_H_
#define _LA_POSTAGGERENGLISH_H_

#include <boost/lexical_cast.hpp>
#include <pos/maxent.h>
#include <pos/common.h>
#include <pos/bidir.h>
#include <Term.h>
#include <Singleton.h>



namespace la
{

    class POSTaggerEnglish: public Singleton<POSTaggerEnglish,std::string>
    {

        private:
            std::vector<ME_Model> vme_;
            POSTaggerEnglish(std::string* dir)
            {
                cout << "Loading English POS model..." ;
                vme_.resize(16);
                for (int i = 0; i < 16; i++)
                    vme_[i].load_from_file(*dir + "/model.bidir." + boost::lexical_cast<std::string>(i));
                cout << "finish!" << endl;
            }
        public:
            void tag(TermList & input);

            // TEMPORARY. If the EnglishAnalyzer class will not be expanded to something more than a tagger, 
            // then it should be Filter 
            void tag(const TermList & input, TermList & output );

            void tag( Term & term );

            friend class Singleton<POSTaggerEnglish,std::string>;
            //	DECLARE_SINGLETON_CLASS(POSTaggerEnglish,std::string);
    };

}
#endif /* _LA_POSTAGGERENGLISH_H_ */
