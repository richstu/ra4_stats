#include "utilities.hpp"

#include <string>
#include <vector>

#include "TTree.h"
#include "TH1D.h"

#include "RooWorkspace.h"

using namespace std;

bool Contains(const string &str, const string &pat){
  return str.find(pat) != string::npos;
}

void ReplaceAll(string &str, const string &orig, const string &rep){
  size_t loc = 0;
  while ((loc = str.find(orig, loc)) != string::npos) {
    str.replace(loc, orig.length(), rep);
    loc += rep.length();
  }
}

void RmCutOn(string &cut, const string &to_rm, const string &rep){
  size_t loc = 0;
  while ((loc = cut.find(to_rm, loc)) != string::npos) {
    size_t end = cut.find_first_of(")&| ", loc);
    cut.replace(loc, end-loc, rep);
    loc += rep.length();
  }
}

size_t MaxIndex(const vector<double> &v){
  if(v.size() == 0) return -1;
  size_t imax = 0;
  for(size_t i = 1; i < v.size(); ++i){
    if(v.at(i) > v.at(imax)){
      imax = i;
    }
  }
  return imax;
}

void DefineSet(RooWorkspace &w,
               const string &set_name,
               const vector<string> &var_names){
  if(var_names.size()==0){
    w.defineSet(set_name.c_str(), "");
  }else{
    string cat_names = var_names.at(0);
    for(size_t ivar = 1; ivar < var_names.size(); ++ivar){
      cat_names += ("," + var_names.at(ivar));
    }
    w.defineSet(set_name.c_str(), cat_names.c_str());
  }
}

void GetCountAndUncertainty(TTree &tree,
                            const Cut &cut,
                            double &count,
                            double &uncertainty){
  const string hist_name{"temp"};
  TH1D temp{hist_name.c_str(), "", 1, -1.0, 1.0};
  temp.Sumw2();
  tree.Project(hist_name.c_str(), "0.", static_cast<const char *>(cut));
  count=temp.IntegralAndError(0,2,uncertainty);
}
