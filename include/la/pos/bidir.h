/*
 * bidir.h
 *
 *  Created on: 2009-6-30
 *      Author: zhjay
 */

#ifndef BIDIR_H_
#define BIDIR_H_
#include <pos/common.h>

//using namespace std;
namespace la
{

const int UPDATE_WINDOW_SIZE = 2;
//const int BEAM_NUM = 10;
const unsigned int BEAM_NUM = 1;
const double BEAM_WINDOW = 0.01;
//const double BEAM_WINDOW = 0.9;
const bool ONLY_VERTICAL_FEATURES = false;

//extern std::string normalize(const std::string & s);
void tokenize(const std::string & s1, std::list<std::string> & lt);

std::string normalize(const std::string & s);

ME_Sample
mesample(const std::vector<Token> &vt,
		 int i,
         const std::string & pos_left2,
         const std::string & pos_left1,
         const std::string & pos_right1,
         const std::string & pos_right2);

ME_Sample mesample(const std::vector<Token> &vt, int i, const std::string & prepos);

double entropy(const std::vector<double>& v);

int bidir_train(const std::vector<Sentence> & vs, int para);

struct Hypothesis
{
    std::vector<Token> vt;
  std::vector<double> vent;
  std::vector<int> order;
  std::vector< std::vector<std::pair<std::string, double> > > vvp;
  double prob;
  bool operator<(const Hypothesis & h) const {
    return prob < h.prob;
  }
  Hypothesis(const std::vector<Token> & vt_,
             const std::multimap<std::string, std::string> & tagdic,
             const std::vector<ME_Model> & vme)
  {
    prob = 1.0;
    vt = vt_;
    size_t n = vt.size();
    vent.resize(n);
    vvp.resize(n);
    order.resize(n);
    for (size_t i = 0; i < n; i++) {
      vt[i].prd = "";
      Update(i, tagdic, vme);
    }
  }
  void Print()
  {
    for (size_t k = 0; k < vt.size(); k++) {
        std::cout << vt[k].str << "/";
      if (vt[k].prd == "") std::cout << "?";
      else std::cout << vt[k].prd;
      std::cout << " ";
    }
    std::cout << std::endl;
  }
  void Update(const int j,
              const std::multimap<std::string, std::string> & tagdic,
              const std::vector<ME_Model> & vme)
  {
      std::string pos_left1 = "BOS", pos_left2 = "BOS2";
    if (j >= 1) pos_left1 = vt[j-1].prd; // maybe bug??
    //    if (j >= 1 && vt[j-1] != "") pos_left1 = vt[j-1].prd; // this should be correct
    if (j >= 2) pos_left2 = vt[j-2].prd;
    std::string pos_right1 = "EOS", pos_right2 = "EOS2";
    if (j <= int(vt.size()) - 2) pos_right1 = vt[j+1].prd;
    if (j <= int(vt.size()) - 3) pos_right2 = vt[j+2].prd;
    ME_Sample mes = mesample(vt, j, pos_left2, pos_left1, pos_right1, pos_right2);

    std::vector<double> membp;
    const ME_Model * mep = NULL;
    int bits = 0;
    if (pos_left2  != "") bits += 8;
    if (pos_left1  != "") bits += 4;
    if (pos_right1 != "") bits += 2;
    if (pos_right2 != "") bits += 1;
    assert(bits >= 0 && bits < 16);
    mep = &(vme[bits]);
    membp = mep->classify(mes);
    assert(mes.label != "");
    vent[j] = entropy(membp);
    //    vent[j] = -j;

    vvp[j].clear();
    double maxp = membp[mep->get_class_id(mes.label)];
    //    vp[j] = mes.label;
    for (int i = 0; i < mep->num_classes(); i++) {
      double p = membp[i];
      if (p > maxp * BEAM_WINDOW)
        vvp[j].push_back(std::pair<std::string, double>(mep->get_class_label(i), p));
    }
  }
};

struct hashfun_str
{
  size_t operator()(const std::string& s) const {
    assert(sizeof(int) == 4 && sizeof(char) == 1);
    const int* p = reinterpret_cast<const int*>(s.c_str());
    size_t v = 0;
    int n = s.size() / 4;
    for (int i = 0; i < n; i++, p++) {
      //      v ^= *p;
      v ^= *p << (4 * (i % 2)); // note) 0 <= char < 128
    }
    int m = s.size() % 4;
    for (int i = 0; i < m; i++) {
      v ^= s[4 * n + i] << (i * 8);
    }
    return v;
  }
};



void generate_hypotheses(const int order, const Hypothesis & h,
                         const std::multimap<std::string, std::string> & tag_dictionary,
                         const std::vector<ME_Model> & vme,
                         std::list<Hypothesis> & vh);

void
bidir_decode_beam(std::vector<Token> & vt,
                  const std::multimap<std::string, std::string> & tag_dictionary,
                  const std::vector<ME_Model> & vme);

void
decode_no_context(std::vector<Token> & vt, const ME_Model & me_none);

class ParenConverter
{
    std::map<std::string, std::string> ptb2pos;
  std::map<std::string, std::string> pos2ptb;
public:
  ParenConverter() {
    const static char* table[] = {
      "-LRB-", "(",
      "-RRB-", ")",
      "-LSB-", "[",
      "-RSB-", "]",
      "-LCB-", "{",
      "-RCB-", "}",
      "***", "***",
    };

    for (int i = 0;; i++) {
      if (std::string(table[i]) == "***") break;
      ptb2pos.insert(std::make_pair(table[i], table[i+1]));
      pos2ptb.insert(std::make_pair(table[i+1], table[i]));
    }
  }
  std::string Ptb2Pos(const std::string & s) {
      std::map<std::string, std::string>::const_iterator i = ptb2pos.find(s);
    if (i == ptb2pos.end()) return s;
    return i->second;
  }
  std::string Pos2Ptb(const std::string & s) {
      std::map<std::string, std::string>::const_iterator i = pos2ptb.find(s);
    if (i == pos2ptb.end()) return s;
    return i->second;
  }
};

//ParenConverter paren_converter;

std::string
bidir_postag(const std::string & s, const std::vector<ME_Model> & vme);


int push_stop_watch();

void
bidir_postagging(std::vector<Sentence> & vs,
                 const std::multimap<std::string, std::string> & tag_dictionary,
                 const std::vector<ME_Model> & vme);


}
#endif /* BIDIR_H_ */
