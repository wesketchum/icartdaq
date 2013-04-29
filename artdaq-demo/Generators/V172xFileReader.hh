#ifndef artdaq_demo_Generators_V172xFileReader_hh
#define artdaq_demo_Generators_V172xFileReader_hh

#include "fhiclcpp/fwd.h"
#include "artdaq/DAQdata/Fragments.hh"
#include "artdaq/DAQdata/FragmentGenerator.hh"

#include <string>
#include <vector>

namespace demo {
  // V172xFileReader reads DS50 events from a file or set of files.

  class V172xFileReader : public artdaq::FragmentGenerator {
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
}

#endif /* artdaq_demo_Generators_V172xFileReader_hh */
