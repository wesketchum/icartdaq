#ifndef ds50daq_DAQ_V172xFileReader_hh
#define ds50daq_DAQ_V172xFileReader_hh

#include "fhiclcpp/fwd.h"
#include "artdaq/DAQdata/Fragments.hh"
#include "artdaq/DAQdata/FragmentGenerator.hh"

#include <string>
#include <vector>

namespace ds50 {
  // V172xFileReader reads DS50 events from a file or set of files.
  class V172xFileReader;
}

class ds50::V172xFileReader : public artdaq::FragmentGenerator {
public:
  explicit V172xFileReader(fhicl::ParameterSet const &);

private:
  virtual bool getNext_(artdaq::FragmentPtrs & output);

  // Configuration.
  std::vector<std::string> const fileNames_;
  uint64_t const max_set_size_bytes_;

  // State
  std::pair<std::vector<std::string>::const_iterator, uint64_t> next_point_;

};

#endif /* ds50daq_DAQ_V172xFileReader_hh */
