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

bool StartsWith(const string &str, const string &pat){
  return str.find(pat) == 0;
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

string execute(const string &cmd){
  FILE *pipe = popen(cmd.c_str(), "r");
  if(!pipe) throw runtime_error("Could not open pipe.");
  const size_t buffer_size = 128;
  char buffer[buffer_size];
  string result = "";
  while(!feof(pipe)){
    if(fgets(buffer, buffer_size, pipe) != NULL) result += buffer;
  }

  pclose(pipe);
  return result;
}

vector<string> Tokenize(const string& input,
                        const string& tokens){
  char* ipt(new char[input.size()+1]);
  memcpy(ipt, input.data(), input.size());
  ipt[input.size()]=static_cast<char>(0);
  char* ptr(strtok(ipt, tokens.c_str()));
  vector<string> output(0);
  while(ptr!=NULL){
    output.push_back(ptr);
    ptr=strtok(NULL, tokens.c_str());
  }
  return output;
}
