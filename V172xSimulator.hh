#ifndef ds50daq_DAQ_V172xSimulator_hh
#define ds50daq_DAQ_V172xSimulator_hh

#include "fhiclcpp/fwd.h"
#include "artdaq/DAQdata/Fragments.hh"
#include "artdaq/DAQdata/FragmentGenerator.hh"
#include "ds50daq/DAQ/V172xFragment.hh"

#include <random>
#include <vector>

namespace ds50 {
  class V172xSimulator;
}

// V172xSimulator creates simulated DS50 events, with data
// distributed according to a "histogram" provided in the configuration
// data.

class ds50::V172xSimulator : public artdaq::FragmentGenerator {
public:
  explicit V172xSimulator(fhicl::ParameterSet const & ps);

private:
  virtual bool getNext_(artdaq::FragmentPtrs & output);

  std::size_t current_event_num_;
  std::size_t const events_to_generate_; // go forever if this is 0
  std::size_t const fragments_per_event_;
  std::size_t const starting_fragment_id_;
  std::size_t const nChannels_;
  int const adc_bits_;
  artdaq::RawDataType const run_number_;
  std::mt19937 engine_;
  std::vector<std::vector<size_t>> adc_freqs_;
  std::vector<std::discrete_distribution<V172xFragment::adc_type>> content_generator_;
};

#endif /* ds50daq_DAQ_V172xSimulator_hh */
