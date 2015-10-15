#include "yield_manager.hpp"

#include <iostream>
#include <sstream>

#include "bin.hpp"
#include "process.hpp"
#include "cut.hpp"

using namespace std;

map<YieldKey, GammaParams> YieldManager::yields_ = map<YieldKey, GammaParams>();
const double YieldManager::store_lumi_ = 4.;

YieldManager::YieldManager(double lumi):
  local_lumi_(lumi),
  verbose_(false){
}

GammaParams YieldManager::GetYield(const YieldKey &key) const{
  if(!HaveYield(key)) ComputeYield(key);

  return (local_lumi_/store_lumi_)*yields_.at(key);
}

GammaParams YieldManager::GetYield(const Bin &bin,
				   const Process &process,
				   const Cut &cut) const{
  return GetYield(YieldKey(bin, process, cut));
}

const double & YieldManager::Luminosity() const{
  return local_lumi_;
}

double & YieldManager::Luminosity(){
  return local_lumi_;
}

bool YieldManager::HaveYield(const YieldKey &key) const{
  return yields_.find(key) != yields_.end();
}

void YieldManager::ComputeYield(const YieldKey &key) const{
  const Bin &bin = GetBin(key);
  const Process &process = GetProcess(key);
  const Cut &cut = GetCut(key);

  GammaParams gps;
  bool new_entry = false;

  if(HaveYield(key)){
    if(verbose_){
      cout << "Using known yield for " << key << endl;
    }
    gps = GetYield(key);
  }else if(process.GetEntries() == 0){
    if(verbose_){
      cout << "No entries found for " << key << endl;
    }
    gps.SetNEffectiveAndWeight(0., 0.);
  }else{
    cout << "Computing yield for " << key << endl;
    new_entry = true;
    ostringstream oss;
    oss << local_lumi_ << flush;
    Cut lumi_weight = Cut(oss.str()+"*weight");

    array<Cut, 6> cuts;
    cuts.at(0) = lumi_weight*(cut && bin.Cut() && process.Cut());
    cuts.at(1) = lumi_weight*(cut && process.Cut());
    cuts.at(2) = lumi_weight*(process.Cut());
    cuts.at(3) = lumi_weight;
    cuts.at(4) = Cut(oss.str());
    cuts.at(5) = Cut();

    for(size_t icut = 0; icut < cuts.size() && gps.Weight()<=0.; ++icut){
      if(icut > 0 && !process.CountZeros()){
        gps.SetNEffectiveAndWeight(0., 0.);
        break;
      }
      Cut &this_cut = cuts.at(icut);
      if(verbose_){
	cout << "Trying cut " << this_cut << endl;
      }
      GammaParams temp_gps = process.GetYield(this_cut);
      if(icut == 0) gps = temp_gps;
      else gps.SetNEffectiveAndWeight(0., temp_gps.Weight());
    }
  }

  if(verbose_ || new_entry){
    cout << "Found yield=" << gps << '\n' << endl;
  }
  yields_[key] = (store_lumi_/local_lumi_)*gps;
}