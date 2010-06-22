/*
 * $Id: bidir.cpp,v 1.8 2005/02/16 10:45:39 tsuruoka Exp $
 */

#include <sys/time.h>
#include <stdio.h>
#include <fstream>
#include <map>
#include <list>
#include <iostream>
#include <sstream>
#include <cmath>
#include <set>
#include <la/pos/bidir.h>
#include <la/pos/maxent.h>
#include <la/pos/common.h>

using namespace std;

namespace la
{

//extern string normalize(const string & s);
void tokenize(const string & s1, list<string> & lt);


ME_Sample mesample(const vector<Token> &vt,
		 int i,
         const string & pos_left2,
         const string & pos_left1,
         const string & pos_right1,
         const string & pos_right2)
{
  ME_Sample sample;

  string str = vt[i].str;

  sample.label = vt[i].pos;

  sample.features.push_back("W0_" + str);
  string prestr = "BOS";
  if (i > 0) prestr = vt[i-1].str;
  //  string prestr2 = "BOS2";
  //  if (i > 1) prestr2 = normalize(vt[i-2].str);
  string poststr = "EOS";
  if (i < (int)vt.size()-1) poststr = vt[i+1].str;
  //  string poststr2 = "EOS2";
  //  if (i < (int)vt.size()-2) poststr2 = normalize(vt[i+2].str);

  if (!ONLY_VERTICAL_FEATURES) {
    sample.features.push_back("W-1_" + prestr);
    sample.features.push_back("W+1_" + poststr);

    sample.features.push_back("W-10_" + prestr + "_" + str);
    sample.features.push_back("W0+1_" + str  + "_" + poststr);
  }

  for (unsigned int j = 1; j <= 10; j++) {
    char buf[1000];
    if (str.size() >= j) {
      sprintf(buf, "suf%d_%s", j, str.substr(str.size() - j).c_str());
      sample.features.push_back(buf);
    }
    if (str.size() >= j) {
      sprintf(buf, "pre%d_%s", j, str.substr(0, j).c_str());
      sample.features.push_back(buf);
    }
  }
  // L
  if (pos_left1 != "") {
    sample.features.push_back("P-1_" + pos_left1);
    sample.features.push_back("P-1W0_"  + pos_left1 + "_" + str);
  }
  // L2
  if (pos_left2 != "") {
    sample.features.push_back("P-2_" + pos_left2);
  }
  // R
  if (pos_right1 != "") {
    sample.features.push_back("P+1_" + pos_right1);
    sample.features.push_back("P+1W0_"  + pos_right1 + "_" + str);
  }
  // R2
  if (pos_right2 != "") {
    sample.features.push_back("P+2_" + pos_right2);
  }
  // LR
  if (pos_left1 != "" && pos_right1 != "") {
    sample.features.push_back("P-1+1_" + pos_left1 + "_" + pos_right1);
    sample.features.push_back("P-1W0P+1_"  + pos_left1 + "_" + str + "_" + pos_right1);
  }
  // LL
  if (pos_left1 != "" && pos_left2 != "") {
    sample.features.push_back("P-2-1_" + pos_left2 + "_" + pos_left1);
    //    sample.features.push_back("P-1W0_"  + pos_left + "_" + str);
  }
  // RR
  if (pos_right1 != "" && pos_right2 != "") {
    sample.features.push_back("P+1+2_" + pos_right1 + "_" + pos_right2);
    //    sample.features.push_back("P-1W0_"  + pos_left + "_" + str);
  }
  // LLR
  if (pos_left1 != "" && pos_left2 != "" && pos_right1 != "") {
    sample.features.push_back("P-2-1+1_" + pos_left2 + "_" + pos_left1 + "_" + pos_right1);
    //    sample.features.push_back("P-1W0_"  + pos_left + "_" + str);
  }
  // LRR
  if (pos_left1 != "" && pos_right1 != "" && pos_right2 != "") {
    sample.features.push_back("P-1+1+2_" + pos_left1 + "_" + pos_right1 + "_" + pos_right2);
    //    sample.features.push_back("P-1W0_"  + pos_left + "_" + str);
  }
  // LLRR
  if (pos_left2 != "" && pos_left1 != "" && pos_right1 != "" && pos_right2 != "") {
    sample.features.push_back("P-2-1+1+2_" + pos_left2 + "_" + pos_left1 + "_" + pos_right1 + "_" + pos_right2);
    //    sample.features.push_back("P-1W0_"  + pos_left + "_" + str);
  }

  for (unsigned int j = 0; j < str.size(); j++) {
    if (isdigit(str[j])) {
      sample.features.push_back("CONTAIN_NUMBER");
      break;
    }
  }
  for (unsigned int j = 0; j < str.size(); j++) {
    if (isupper(str[j])) {
      sample.features.push_back("CONTAIN_UPPER");
      break;
    }
  }
  for (unsigned int j = 0; j < str.size(); j++) {
    if (str[j] == '-') {
      sample.features.push_back("CONTAIN_HYPHEN");
      break;
    }
  }

  bool allupper = true;
  for (unsigned int j = 0; j < str.size(); j++) {
    if (!isupper(str[j])) {
      allupper = false;
      break;
    }
  }
  if (allupper)
    sample.features.push_back("ALL_UPPER");

  return sample;
}

ME_Sample mesample(const vector<Token> &vt, int i, const string & prepos)
         //         const string & prepos2)
{
  ME_Sample sample;

  string str = vt[i].str;

  sample.label = normalize(vt[i].pos);

  sample.features.push_back("W0_" + str);
  string prestr = "BOS";
  if (i > 0) prestr = normalize(vt[i-1].str);
  string prestr2 = "BOS2";
  if (i > 1) prestr2 = normalize(vt[i-2].str);
  string poststr = "EOS";
  if (i < (int)vt.size()-1) poststr = normalize(vt[i+1].str);
  string poststr2 = "EOS2";
  if (i < (int)vt.size()-2) poststr2 = normalize(vt[i+2].str);
  sample.features.push_back("W-1_" + prestr);
  sample.features.push_back("W+1_" + poststr);

  sample.features.push_back("W-10_" + prestr + "_" + str);
  sample.features.push_back("W0+1_" + str  + "_" + poststr);

  if (str.size() >= 3)
    sample.features.push_back("suf2_" + str.substr(str.size() - 2));
  if (str.size() >= 4)
    sample.features.push_back("suf3_" + str.substr(str.size() - 3));
  if (str.size() >= 5)
    sample.features.push_back("suf4_" + str.substr(str.size() - 4));
  if (str.size() >= 6)
    sample.features.push_back("suf5_" + str.substr(str.size() - 5));
  sample.features.push_back("P-1_" + prepos);
  sample.features.push_back("P-1W0_"  + prepos + "_" + str);

  if (isupper(str[0]))
    sample.features.push_back("ISUPPER");
  return sample;
}

double entropy(const vector<double>& v)
{
  double sum = 0, maxp = 0;
  for (unsigned int i = 0; i < v.size(); i++) {
    if (v[i] == 0) continue;
    sum += v[i] * log(v[i]);
    maxp = max(maxp, v[i]);
  }
  return -maxp;
//    return -sum;
}

int
bidir_train(const vector<Sentence> & vs, int para)
{
  //  vme.clear();
  //  vme.resize(16);

  for (int t = 0; t < 16; t++) {
    if (t != 15 && t != 0) continue;
  //  for (int t = 15; t >= 0; t--) {
    vector<ME_Sample> train;

    if (para != -1 && t % 4 != para) continue;
    //    if (t % 2 == 1) continue;
    cerr << "type = " << t << endl;
    cerr << "extracting features...";
    //int n = 0; unused
    for (vector<Sentence>::const_iterator i = vs.begin(); i != vs.end(); i++) {
      const Sentence & s = *i;
      for (unsigned int j = 0; j < s.size(); j++) {

        string pos_left1 = "BOS", pos_left2 = "BOS2";
        if (j >= 1) pos_left1 = s[j-1].pos;
        if (j >= 2) pos_left2 = s[j-2].pos;
        string pos_right1 = "EOS", pos_right2 = "EOS2";
        if (j <= (s.size()) - 2) pos_right1 = s[j+1].pos;
        if (j <= (s.size()) - 3) pos_right2 = s[j+2].pos;
        if ( (t & 0x8) == 0 ) pos_left2 = "";
        if ( (t & 0x4) == 0 ) pos_left1 = "";
        if ( (t & 0x2) == 0 ) pos_right1 = "";
        if ( (t & 0x1) == 0 ) pos_right2 = "";

        train.push_back(mesample(s, j, pos_left2, pos_left1, pos_right1, pos_right2));
      }
      //      if (n++ > 1000) break;
    }
    cerr << "done" << endl;

    ME_Model m;
    //    m.set_heldout(1000,0);
    //    m.train(train, 2, 1000, 0);
    m.train(train, 2, 0, 1);
    char buf[1000];
    sprintf(buf, "model.bidir.%d", t);
    m.save_to_file(buf);

  }

  return 1;
}



void generate_hypotheses(const int order, const Hypothesis & h,
                         const multimap<string, string> & tag_dictionary,
                         const vector<ME_Model> & vme,
                         list<Hypothesis> & vh)
{
  int n = h.vt.size();
  int pred_position = -1;
  double min_ent = 999999;
  string pred = "";
  //double pred_prob = 0; unused
  for (int j = 0; j < n; j++) {
    if (h.vt[j].prd != "") continue;
    double ent = h.vent[j];
    if (ent < min_ent) {
      //        pred = h.vvp[j].begin()->first;
      //        pred_prob = h.vvp[j].begin()->second;
      min_ent = ent;
      pred_position = j;
    }
  }
  assert(pred_position >= 0 && pred_position < n);

  for (vector<pair<string, double> >::const_iterator k = h.vvp[pred_position].begin();
       k != h.vvp[pred_position].end(); k++) {
    Hypothesis newh = h;

    newh.vt[pred_position].prd = k->first;
    newh.order[pred_position] = order + 1;
    newh.prob = h.prob * k->second;

    // update the neighboring predictions
    for (int j = pred_position - UPDATE_WINDOW_SIZE; j <= pred_position + UPDATE_WINDOW_SIZE; j++) {
      if (j < 0 || j > n-1) continue;
      if (newh.vt[j].prd == "") newh.Update(j, tag_dictionary, vme);
    }
    vh.push_back(newh);
  }


}

void bidir_decode_beam(vector<Token> & vt,
                  const multimap<string, string> & tag_dictionary,
                  const vector<ME_Model> & vme)
{
  unsigned int n = vt.size();
  if (n == 0) return;

  list<Hypothesis> vh;
  Hypothesis h(vt, tag_dictionary, vme);
  vh.push_back(h);

  for (size_t i = 0; i < n; i++) {
    list<Hypothesis> newvh;
    for (list<Hypothesis>::const_iterator j = vh.begin(); j != vh.end(); j++) {
      generate_hypotheses(i, *j, tag_dictionary, vme, newvh);
    }
    newvh.sort();
    while (newvh.size() > BEAM_NUM) {
      newvh.pop_front();
    }
    vh = newvh;
  }

  h = vh.back();
  for (size_t k = 0; k < n; k++) {
    //    cout << h.vt[k].str << "/" << h.vt[k].prd << "/" << h.order[k] << " ";
    vt[k].prd = h.vt[k].prd;
  }
  //  cout << endl;


}

void
decode_no_context(vector<Token> & vt, const ME_Model & me_none)
{
  unsigned int n = vt.size();
  if (n == 0) return;

  for (size_t i = 0; i < n; i++) {
    ME_Sample mes = mesample(vt, i, "", "", "", "");
    me_none.classify(mes);
    vt[i].prd = mes.label;
  }

  for (size_t k = 0; k < n; k++) {
    cout << vt[k].str << "/" << vt[k].prd << " ";
  }
  cout << endl;

}



string
bidir_postag(const string & s, const vector<ME_Model> & vme)
{
  list<string> lt;
  tokenize(s, lt);
  /*
  istringstream is(s);
  string t;
  while (is >> t) {
    lt.push_back(t);
  }
  */

  vector<Token> vt;
  for (list<string>::const_iterator i = lt.begin(); i != lt.end(); i++) {
    string s = *i;
    //    s = paren_converter.Ptb2Pos(s);
    vt.push_back(Token(s, "?"));
  }

  const multimap<string, string> dummy;
  //  bidir_decode_search(vt, dummy, vme);
  bidir_decode_beam(vt, dummy, vme);

  string tmp;
  for (size_t i = 0; i < vt.size(); i++) {
    string s = vt[i].str;
    string p = vt[i].prd;
    //     s = paren_converter.Pos2Ptb(s);
    //    p = paren_converter.Pos2Ptb(p);
    if (i == 0) tmp += s + "/" + p;
    else        tmp += " " + s + "/" + p;
  }
  return tmp;
}


int push_stop_watch()
{
  static struct timeval start_time, end_time;
  static bool start = true;
  if (start) {
    gettimeofday(&start_time, NULL);
    start = false;
    return 0;
  }

  gettimeofday(&end_time, NULL);
  int elapse_msec = (end_time.tv_sec - start_time.tv_sec) * 1000 +
    (int)((end_time.tv_usec - start_time.tv_usec) * 0.001);
  cerr << elapse_msec << " msec" << endl;
  start = true;
  return elapse_msec;
}

void
bidir_postagging(vector<Sentence> & vs,
                 const multimap<string, string> & tag_dictionary,
                 const vector<ME_Model> & vme)
{
  //int num_classes = vme[0].num_classes(); unused

  cerr << "now tagging";
  push_stop_watch();
  int n = 0, ntokens = 0;
  for (vector<Sentence>::iterator i = vs.begin(); i != vs.end(); i++) {
    Sentence & s = *i;
    ntokens += s.size();
    //    if (s.size() > 2) continue;
    bidir_decode_beam(s, tag_dictionary, vme);
    //    bidir_decode_search(s, tag_dictionary, vme);
    //decode_no_context(s, vme[0]);

    //    cout << n << endl;
    for (size_t k = 0; k < s.size(); k++) {
      cout << s[k].str << "/" << s[k].prd << " ";
    }
    cout << endl;
    //    if (n > 100) break;

    if (n++ % 10 == 0) cerr << ".";
  }
  cerr << endl;
  int msec = push_stop_watch();
  cerr << ntokens / (msec/1000.0) << " tokens / sec" << endl;
}

}


