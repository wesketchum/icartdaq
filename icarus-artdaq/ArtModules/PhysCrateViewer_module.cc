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

  
  for (size_t idx = 0; idx < raw_data->size(); ++idx) {
    const auto& frag((*raw_data)[idx]);
    
    PhysCrateFragment bb(frag);
    for(size_t i_b=0; i_b < bb.nBoards(); ++i_b){
      std::stringstream ss_hist_title,ss_hist_name;
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

      std::cout << "going to create histogram " << ss_hist_name.str() << std::endl;

      hist_vector.push_back(tfs->make<TH2F>(ss_hist_name.str().c_str(),ss_hist_title.str().c_str(),
					    bb.nChannelsPerBoard(),0,bb.nChannelsPerBoard(),
					    bb.nSamplesPerChannel(),0,bb.nSamplesPerChannel()));

      std::cout << "Created histo. Total histos is now " << hist_vector.size() << std::endl;

      
      std::cout << "Printing board " << i_b+1 << " / " << bb.nBoards() << std::endl;
      std::cout << "\tData location is " << bb.DataTileHeaderLocation(i_b) << "." << std::endl;
      std::cout << "\t Board (event,timestamp) = (0x" << std::hex << bb.BoardEventNumber(i_b) << ", 0x"
		<< bb.BoardTimeStamp(i_b) << ")" << std::dec << std::endl;
      std::cout << "\t(First data word is 0x"
		<< std::hex << *(bb.BoardData(i_b)) << std::dec << ")" << std::endl;
      for(size_t i_t=0; i_t<bb.nSamplesPerChannel(); ++i_t){
	//std::cout << "Printing sample " << i_t+1 << " / " << bb.nSamplesPerChannel() << std::endl;
	for(size_t i_c=0; i_c<bb.nChannelsPerBoard(); ++i_c){
	  //std::cout << "\tPrinting channel " << i_c+1 << " / " << bb.nChannelsPerBoard() << std::endl;
	  auto adc_value = bb.adc_val(i_b,i_c,i_t);
	  if(adc_value!=0) hist_vector.back()->SetBinContent(i_c+1,i_t+1,adc_value);
	}
      }
    }

  }
  
}

DEFINE_ART_MODULE(icarus::PhysCrateViewer)
