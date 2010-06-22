/**
 * @file    ChineseLanguageAction.h
 * @author  Vernkin
 * @date    Oct 9, 2009
 * @details
 *  LanguageAction for Chinese language.
 */

#ifdef USE_IZENECMA
#ifndef CHINESELANGUAGEACTION_H_
#define CHINESELANGUAGEACTION_H_

#include "la/CommonLanguageAnalyzer.h"

#include <analyzer.h>
#include <knowledge.h>
#include <sentence.h>

#include <iostream>

namespace la
{
// Foreign Language is Chinese POS
#define CHINESE_FL "NX"

// compound noun in Chinese POS
#define CHINESE_NNP "N"

// Person's name in Chinese POS
#define CHINESE_NNI "NR"

// Number in Chinese POS
#define CHINESE_SN "M"

class ChineseLanguageAction {
public:
    /**
     * \param loadModel whether load loadModel.
     */
    ChineseLanguageAction( const string& knowledgePath, bool loadModel = true );
    virtual ~ChineseLanguageAction();

    inline int getFLMorp()
    {
        return pA_->getCodeFromStr( CHINESE_FL );
    }

    inline string getFLPOS()
    {
        return CHINESE_FL;
    }

    inline int getNNIMorp()
    {
        return pA_->getCodeFromStr( CHINESE_NNI );
    }

    inline string getNNIPOS()
    {
        return CHINESE_NNI;
    }

    inline int getNNPMorp()
    {
        return pA_->getCodeFromStr( CHINESE_NNP );
    }

    inline string getNNPPOS()
    {
        return CHINESE_NNP;
    }

    inline int getSCMorp()
    {
        return -1; //FIXME no such POS in iCMA
    }

    inline wiselib::UString::EncodingType getEncodeType()
    {
        return wiselib::UString::UTF_8;
    }

    inline void setNBest( unsigned int num=2 )
    {
        pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, num );
    }

    /**
     * @brief  Sets the lower bound digit limit of the numbers extracted, empty body for Chinese.
     */
    inline void setLowDigitBound( unsigned int num=1 ){}

    /**
     * @brief   Whether to combine number dependent noun to extracted term, empty body for Chinese.
     */
    inline void setCombineBoundNoun( bool flag=false ){}

    /**
     * @brief   Whether or not to extract stem terms of verb and adj words, empty body for Chinese.
     */
    inline void setVerbAdjStems( bool flag=false ){}


    inline void setIndexMode()
    {
        if( pA_ == NULL )
        {
            throw std::logic_error( "ChineseAnalyzer::setIndexMode() is call with pA_ NULL" );
        }
        resetAnalyzer();

        //pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, 1 );

        vector<string> posList;
        addDefaultPOSList(posList);

        // unknown
        posList.push_back("un");

        pA_->setIndexPOSList(posList);
    }

    inline void setLabelMode()
    {
        if( pA_ == NULL )
        {
            throw std::logic_error( "ChineseAnalyzer::setLabelMode() is call with pA_ NULL" );
        }
        resetAnalyzer();

        //pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, 1 );

        // to index all the POS
        pA_->resetIndexPOSList( true );
    }

    inline cma::Sentence* getSynBasicSentence( const char* input )
    {
        sensyn_.setString( input );
        pAsyn_->runWithSentence( sensyn_ );
        return &sensyn_;
    }

    inline cma::Sentence* getBasicSentence( const char* input )
    {
        sen_.setString( input );
        pA_->runWithSentence( sen_ );
        return &sen_;
    }

    inline bool isScFlSn( int morp )
    {
        return morp == flMorp_ || morp == snMorp_;
    }

    inline bool isAcceptedNoun( int morp )
    {
        if( morp < 0 || morp >= posSize_ )
            return false;
        return acceptedNouns_[ morp ];
    }

    inline void setCaseSensitive( bool flag = true )
    {
        //TODO no support in iCMA
    }

    /**
     * In the same token, whether each Morpheme shared the same word offset
     */
    bool isSharedWordOffset();

    /**
     * @brief   Set the analysis approach type
     */
    inline void setAnalysisType( unsigned int type = 2 )
    {
        pA_->setOption( cma::Analyzer::OPTION_ANALYSIS_TYPE, type );
    }

private:
    inline void resetAnalyzer()
    {
        //pA_->setOption( cma::Analyzer::OPTION_TYPE_NBEST, 1 );
        pA_->resetIndexPOSList( false );
    }

    inline void addDefaultPOSList( vector<string>& posList )
    {
        // defaulted index POS
        //This POS list should be keeped even if something are commented, hence which one
        // is default or not is clear.
        string defaultPOS[] = {
            "A",  //adjective
            "AD", //Adjective-like adverb
            //"AG", //形语素形容词性语素。形容词代码为a，语素代码ｇ前面置以A
            "AN",  //名形词具有名词功能的形容词。形容词代码a和名词代码n并在一起
            "B",  //区别词:大型一次性短效,取汉字'别'的声母
            //"C",  //conjunction
            //"D",  //adverb
            //"DG",  //副语素副词性语素。副词代码为d，语素代码ｇ前面置以D
            //"E",  //exclamation
            //"F",  //Position, like 'up', 'down'
            //"G",  //语素绝大多数语素都能作为合成词的'词根'，取汉字'根'的声母
            //"H",  //head
            "I",  //Idiom
            "J",  //Abbreviation
            "K",  //appending part
            "L",  //Temporarily used words
            //"M",  //numeral
            //"MQ",  //number and unit
            //"NG",  //名语素名词性语素。名词代码为n，语素代码ｇ前面置以N
            "N",  //noun
            "NR",  //Person's Name
            "NS",  //Space's(Location) Name
            "NT",  //Organization's Name
            CHINESE_FL,  //English words
            "NZ",  //Proper Nouns
            //"O",  //onomatopoeia
            //"P",  //prepositional
            //"Q",  //quantity
            //"R",  //pronoun
            "S",  //Space or Location
            //"TG", // 时语素时间词性语素。时间词代码为t,在语素的代码g前面置以T
            "T",  //time
            //"U",  //auxiliary
            "VD",  //副动词直接作状语的动词。动词和副词的代码并在一起
            //"VG",  //动语素动词性语素。动词代码为v。在语素的代码g前面置以V
            "VN"  //名动词指具有名词功能的动词。动词和名词的代码并在一起
            "V",  //动词verb
            //"W",  //标点符号
            //"X",  //非语素字非语素字只是一个符号，字母x通常用于代表未知数、符号
            //"Y",  //语气词汉字“语”的声母
            //"Z",  //状态词:酷热空空闪闪甜甜取汉字'状'的声母的前一个字母
            "un", // 未知
            "" // Reserved one, don't Remove. It will be ignored when added
        };

        int size = sizeof(defaultPOS) / sizeof(std::string) - 1; // -1 is remove the last one

        for(int i=0; i<size; ++i)
            posList.push_back(defaultPOS[i]);
    }

    /**
     * Invoked by the last step of the constructor
     */
    inline void initAcceptedNouns()
    {
       if( acceptedNouns_ != 0 )
            delete[] acceptedNouns_;
        posSize_ = pA_->getPOSTagSetSize();

        acceptedNouns_ = new bool[ posSize_ ];
        // default all is false
        memset( acceptedNouns_, 0x0, sizeof(bool) * posSize_);

        string acceptedList[] = {"N", "NR", "NS", "NT", "NZ"};
        int size = sizeof(acceptedList) / sizeof(std::string);

        for(int i=0; i<size; ++i)
        {
            int morp = pA_->getCodeFromStr( acceptedList[i] );
            if( morp < 0 || morp >= posSize_)
                std::cerr << "[Warning][ChineseLanguageAction]Can't find POS for " <<
                    acceptedList[i] << ", ret pos Morpheme is " << morp << std::endl;
            else
                acceptedNouns_[ morp ] = true;
        }
    }

private:
    /**
     * Pointer for CMA's Analyzer
     */
    cma::Analyzer* pA_;

    /**
     * Analyzer for processing synonyms
     */
    cma::Analyzer* pAsyn_;

    /**
     * Sentence for pA_
     */
    cma::Sentence sen_;

    /**
     * Sentence for pASyn_
     */
    cma::Sentence sensyn_;

    //local variable, initialied in the constructor after Analyzer
    int flMorp_;
    int snMorp_;

    int posSize_;
    bool* acceptedNouns_;
};


}
#endif /* CHINESELANGUAGEACTION_H_ */
#endif
