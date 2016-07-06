////////////////////////////////////////////////////////////////////////
// Class:       PhysCrateViewer
// Module Type: analyzer
// File:        PhysCrateViewer_module.cc
// Description: Prints out information about each event.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "art/Utilities/Exception.h"
#include "icartdaq-core/Overlays/PhysCrateFragment.hh"
#include "icartdaq-core/Overlays/PhysCrateStatFragment.hh"
#include "artdaq-core/Data/Fragments.hh"

#include "art/Framework/Services/Optional/TFileService.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <sstream>

#include "TH2F.h"

namespace icarus {
  class PhysCrateViewer;
}

class icarus::PhysCrateViewer : public art::EDAnalyzer {
public:
  explicit PhysCrateViewer(fhicl::ParameterSet const & pset);
  virtual ~PhysCrateViewer();

  virtual void analyze(art::Event const & evt);

private:
  std::string raw_data_label_;
};


icarus::PhysCrateViewer::PhysCrateViewer(fhicl::ParameterSet const & pset)
  : EDAnalyzer(pset),
    raw_data_label_(pset.get<std::string>("raw_data_label"))
    
{
}

icarus::PhysCrateViewer::~PhysCrateViewer()
{
}

void icarus::PhysCrateViewer::analyze(art::Event const & evt)
{
  
  art::EventNumber_t eventNumber = evt.event();
  
  // ***********************
  // *** PhysCrate Fragments ***
  // ***********************
  
  // look for raw PhysCrate data
  
  art::Handle< std::vector<artdaq::Fragment> > raw_data;
  evt.getByLabel(raw_data_label_, "PHYSCRATEDATA", raw_data);

  art::Handle< std::vector<artdaq::Fragment> > raw_stat;
  evt.getByLabel(raw_data_label_, "PHYSCRATESTAT", raw_stat);
  
  if(!raw_data.isValid()){
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has zero"
              << " PhysCrate fragments " << " in module " << raw_data_label_ << std::endl;
    std::cout << std::endl;
    return;
  }
  if(!raw_stat.isValid()){
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has zero"
              << " PhysCrateStat fragments " << " in module " << raw_data_label_ << std::endl;
    std::cout << std::endl;
    return;
  }

  art::ServiceHandle<art::TFileService> tfs;
  std::vector<TH2F*> hist_vector;
  
  
  std::cout << "######################################################################" << std::endl;
  std::cout << std::endl;
  std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
	    << ", event " << eventNumber << " has " << raw_data->size()
	    << " PhysCrate fragment(s)." << std::endl;

  std::stringstream ss_hist_title,ss_hist_name;
  
  for (size_t idx = 0; idx < raw_data->size(); ++idx) {
    const auto& frag((*raw_data)[idx]);
    
    PhysCrateFragment bb(frag);
    for(size_t i_b=0; i_b < bb.nBoards(); ++i_b){
      ss_hist_title << "(Run,Event,Fragment,Board)=("
		    << evt.run() <<","
		    << eventNumber << ","
		    << idx << ","
		    << i_b << ");Channel;Time";
      ss_hist_name << "h_"
		   << evt.run() <<"_"
		   << eventNumber << "_"
		   << idx << "_"
		   << i_b;
      hist_vector.push_back(tfs->make<TH2F>(ss_hist_name.str().c_str(),ss_hist_title.str().c_str(),
					    bb.nChannels(),0,bb.nChannels(),
					    bb.nSamplesPerChannel(),0,bb.nSamplesPerChannel()));
      for(size_t i_t=0; i_t<bb.nSamplesPerChannel(); ++i_t)
	for(size_t i_c=0; i_c<bb.nChannels(); ++i_c)
	  hist_vector.back()->SetBinContent(i_c+1,i_t+1,bb.adc_val(i_b,i_c,i_t));
    }

  }
  
}

DEFINE_ART_MODULE(icarus::PhysCrateViewer)
