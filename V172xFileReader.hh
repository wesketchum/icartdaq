#ifndef ds50daq_DAQ_V172xFileReader_hh
#define ds50daq_DAQ_V172xFileReader_hh

#include "fhiclcpp/fwd.h"
#include "artdaq/DAQdata/Fragments.hh"
#include "DS50FragmentGenerator.hh"

#include <string>
#include <vector>

namespace ds50 {
  // V172xFileReader reads DS50 events from a file or set of files.

  class V172xFileReader : public DS50FragmentGenerator {
    public:
      explicit V172xFileReader(fhicl::ParameterSet const &);

    private:
      virtual bool getNext__(artdaq::FragmentPtrs & output);

  // Configuration.
  std::vector<std::string> const fileNames_;
  uint64_t const max_set_size_bytes_;
  int const max_events_;

  // State
  size_t events_read_;
  std::pair<std::vector<std::string>::const_iterator, uint64_t> next_point_;
};
>>>>>>> develop

#endif /* ds50daq_DAQ_V172xFileReader_hh */
