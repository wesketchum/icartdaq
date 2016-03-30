////////////////////////////////////////////////////////////////////////
// Class:       CAEN2795WriteICARUSDataFile
// Module Type: analyzer
// File:        CAEN2795WriteICARUSDataFile_module.cc
// Description: Writes data in a QSCAN ready format, hopefully...
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Principal/Handle.h"

#include "art/Utilities/Exception.h"
#include "icartdaq-core/Overlays/CAEN2795Fragment.hh"
#include "artdaq-core/Data/Fragments.hh"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "packs.h"

#define EVEN 0x4546454E
#define DATA 0x44415441
#define STAT 0x53544154

namespace icarus {
  class CAEN2795WriteICARUSDataFile;
}

class icarus::CAEN2795WriteICARUSDataFile : public art::EDAnalyzer {
public:
  explicit CAEN2795WriteICARUSDataFile(fhicl::ParameterSet const & pset);
  virtual ~CAEN2795WriteICARUSDataFile();

  virtual void analyze(art::Event const & evt);

  virtual void beginSubRun(art::SubRun const&) override;
  virtual void endSubRun(art::SubRun const&) override;

private:
  std::string raw_data_label_;
  std::string file_output_name_;
  std::string file_output_location_;

  std::ofstream output_file_;
};


icarus::CAEN2795WriteICARUSDataFile::CAEN2795WriteICARUSDataFile(fhicl::ParameterSet const & pset)
  : EDAnalyzer(pset),
    raw_data_label_(pset.get<std::string>("raw_data_label")),
    file_output_name_(pset.get<std::string>("file_output_name")),
    file_output_location_(pset.get<std::string>("file_output_location"))
    
{
}

icarus::CAEN2795WriteICARUSDataFile::~CAEN2795WriteICARUSDataFile()
{
  
}

void icarus::CAEN2795WriteICARUSDataFile::beginSubRun(art::SubRun const& subrun)
{

  std::stringstream fn;
  fn << file_output_location_ << "/" << file_output_name_ << "_"
     << subrun.run() << "_" << subrun.subRun() << ".qscan";
  output_file_.open(fn.str(),std::ofstream::binary | std::ofstream::out);
}
void icarus::CAEN2795WriteICARUSDataFile::endSubRun(art::SubRun const&)
{
  output_file_.close();
}

void icarus::CAEN2795WriteICARUSDataFile::analyze(art::Event const & evt)
{
  
  art::EventNumber_t eventNumber = evt.event();
  
  // ***********************
  // *** CAEN2795 Fragments ***
  // ***********************
  
  // look for raw CAEN2795 data
  
  art::Handle< std::vector<artdaq::Fragment> > raw;
  evt.getByLabel(raw_data_label_, "CAEN2795", raw);
  
  if(!raw.isValid()){
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has zero"
              << " CAEN2795 fragments " << " in module " << raw_data_label_ << std::endl;
    std::cout << std::endl;
    return;
  }
  
  
  std::cout << "######################################################################" << std::endl;
  std::cout << std::endl;
  std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
	    << ", event " << eventNumber << " has " << raw->size()
	    << " CAEN2795 fragment(s)." << std::endl;

  evHead header;
  header.token = (int)EVEN;
  header.Run = (int)evt.run();
  header.Event = (int)eventNumber;
  header.ToD = 0;
  header.AbsTime = 0;
  header.Conf = 0;
  header.Size = 28;

  output_file_.write((char*)&header,sizeof(evHead));
  
  for (size_t idx = 0; idx < raw->size(); ++idx) {
    const auto& frag((*raw)[idx]);
    output_file_.write((char*)frag.dataBeginBytes(),frag.dataSizeBytes());
  }

}


DEFINE_ART_MODULE(icarus::CAEN2795WriteICARUSDataFile)
