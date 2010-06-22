/*
 * $Id: postag.cpp,v 1.5 2004/12/21 13:54:46 tsuruoka Exp $
 */

#include <stdio.h>
#include <fstream>
#include <map>
#include <list>
#include <iostream>
#include <sstream>
#include <cmath>
#include <set>
#include <pos/maxent.h>
#include <pos/common.h>
#include <pos/bidir.h>
using namespace std;

namespace la
{
const double BEAM_WIDTH = 3.0;


string
normalize(const string & s)
{
  string tmp(s);
  for (size_t i = 0; i < tmp.size(); i++) {
    if (isdigit(tmp[i])) tmp[i] = '#';
  }
  return tmp;
}

void
viterbi(vector<Token> & vt, const ME_Model & me)
{
  if (vt.size() == 0) return;

  vector< vector<double> > mat;
  vector< vector<int> > bpm;

  vector<double> vd(me.num_classes());
  for (size_t j = 0; j < vd.size(); j++) vd[j] = 0;

  mat.push_back(vd);

  for (size_t i = 0; i < vt.size(); i++) {

    vector<double> vd(me.num_classes());
    for (size_t j = 0; j < vd.size(); j++) vd[j] = -999999;
    vector<int> bp(me.num_classes());

    double maxl = -999999;
    for (size_t j = 0; j < vd.size(); j++) {
      if (mat[i][j] > maxl) maxl = mat[i][j];
    }

    for (size_t j = 0; j < vd.size(); j++) {
      if (mat[i][j] < maxl - BEAM_WIDTH) continue; // beam thresholding

      string prepos = me.get_class_label(j);
      if (i == 0) {
        if (j > 0) continue;
        prepos = "BOS";
      }
      //      prepos = me.get_class_name(j);
      //      if (i == 0 && prepos != "BOS") continue;

      ME_Sample mes = mesample(vt, i, prepos);
      vector<double> membp = me.classify(mes);
      for (size_t k = 0; k < vd.size(); k++) {
        double l = mat[i][j] + log(membp[k]);
        if (l > vd[k]) {
          bp[k] = j;
          vd[k] = l;
        }
      }
    }
    mat.push_back(vd);
    //    for (int k = 0; k < vd.size(); k++) cout << bp[k] << " ";
    //    cout << endl;
    bpm.push_back(bp);
  }
  //  cout << "viterbi ";
  int max_prd = 0;
  int n = vt.size();
  for (size_t j = 0; j < vd.size(); j++) {
    double l = mat[n][j];
    if (l > mat[n][max_prd]) {
      max_prd = j;
    }
  }
  vt[n-1].prd = me.get_class_label(max_prd);
  for (int i = vt.size() - 2; i >= 0; i--) {
    //    cout << max_prd << " ";
    //    cerr << max_prd << " ";
    if (max_prd < 0 || max_prd >= me.num_classes()) exit(0);
    max_prd = bpm[i+1][max_prd];
    vt[i].prd = me.get_class_label(max_prd);
  }
  //  cout << endl;

}

string postag(const string & s, const ME_Model & me)
{
  list<string> lt;
  tokenize(s, lt);

  vector<Token> vt;
  for (list<string>::const_iterator i = lt.begin(); i != lt.end(); i++) {
    vt.push_back(Token(*i, "?"));
  }

  viterbi(vt, me);

  string tmp;
  for (size_t i = 0; i < vt.size(); i++) {
    if (i == 0) tmp += vt[i].str + "/" + vt[i].prd;
    else        tmp += " " + vt[i].str + "/" + vt[i].prd;
  }
  return tmp;
}
}

/*
 * $Log: postag.cpp,v $
 * Revision 1.5  2004/12/21 13:54:46  tsuruoka
 * add bidir.cpp
 *
 * Revision 1.4  2004/12/20 12:06:24  tsuruoka
 * change the data
 *
 * Revision 1.3  2004/07/29 12:40:33  tsuruoka
 * modify features
 *
 * Revision 1.2  2004/07/26 03:46:48  tsuruoka
 * add confusing_word_table
 *
 * Revision 1.1  2004/07/16 13:40:42  tsuruoka
 * init
 *
 */

