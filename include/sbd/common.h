#ifndef _SBD_COMMON_H_
#define _SBD_COMMON_H_

#include <iostream>
#include <fstream>
#include <set>
#include <tr1/unordered_map>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>
#include "collocation.h"

using namespace std;


/**
 * Some common data structue and functions
 *
 * @author Eric - 2010.08.22
 *
 */
namespace sbd 
{


const string PERIOD = ".";

// frequency distribution type
typedef std::tr1::unordered_map<string, size_t> Fdist;

// collocation distribution type
typedef std::map<Collocation, size_t> CollocationFdist;

// 
typedef std::tr1::unordered_map<string, size_t> OrthMap;

typedef boost::tokenizer<boost::char_separator<char> > BoostTokenizer;


////////////////////////////////////////////////////
// orthographic context constants
////////////////////////////////////////////////////

/**
 * The following constants are used to describe the orthographic
 * contexts in which a word can occur.  BEG=beginning, MID=middle,
 * UNK=unknown, UC=uppercase, LC=lowercase, NC=no case.
 */
extern int ORTHO_BEG_UC;
extern int ORTHO_MID_UC;
extern int ORTHO_UNK_UC;
extern int ORTHO_BEG_LC;
extern int ORTHO_MID_LC;
extern  int ORTHO_UNK_LC;

extern int ORTHO_UC;
extern int ORTHO_LC;

const string INITIAL  = "initial";
const string INTERNAL = "internal";
const string UNKNOWN  = "unknown";

const string UPPER    = "upper";
const string LOWER    = "lower";

const string NONE     = "none";


int getOrthConst(string pos, string wordcase);

////////////////////////////////////////////////////
// frequency distribution functions
////////////////////////////////////////////////////

void insertFdist(Fdist& fdist, string str);
int getFdist(Fdist& fdist, string str);
void insertCollocationFdist(CollocationFdist& fdist, 
        Collocation& coll);


}

#endif
