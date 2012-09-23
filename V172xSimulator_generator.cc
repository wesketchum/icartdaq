#include "ds50daq/DAQ/V172xSimulator.hh"

#include "artdaq/DAQdata/GeneratorMacros.hh"
#include "cetlib/exception.h"
#include "ds50daq/DAQ/V172xFragment.hh"
#include "ds50daq/DAQ/V172xFragmentWriter.hh"
#include "fhiclcpp/ParameterSet.h"

#include <fstream>
#include <iomanip>
#include <iterator>

using namespace artdaq;

namespace {
  void read_adc_freqs(std::string const & fileName,
                      std::vector<std::vector<size_t>> & freqs, int adc_bits) {
    std::ifstream is(fileName);
    if (!is) {
      throw cet::exception("FileOpenError")
        << "Unable to open distribution data file "
        << fileName;
    }
    std::string header;
    std::getline(is, header);
    is.ignore(1);
    std::vector<std::string> split_headers;
    cet::split(header, ' ', std::back_inserter(split_headers));
    size_t nHeaders = split_headers.size();
    freqs.clear();
    freqs.resize(nHeaders -1); // Take account of ADC column.
    for (auto & freq : freqs) {
      freq.resize(ds50::V172xFragment::adc_range(adc_bits));
    }
    while (is.peek() != EOF) { // If we get EOF here, we're OK.
      if (!is.good()) {
        throw cet::exception("FileReadError")
          << "Error reading distribution data file "
          << fileName;
      }
      ds50::V172xFragment::adc_type channel;
      is >> channel;
      for (auto & freq : freqs) {
        is >> freq[channel];
      }
      is.ignore(1);
    }
    is.close();
  }
}

ds50::V172xSimulator::V172xSimulator(fhicl::ParameterSet const & ps):
  current_event_num_(0),
  events_to_generate_(ps.get<size_t>("events_to_generate", 0)),
  fragments_per_event_(ps.get<size_t>("fragments_per_event", 5)),
  starting_fragment_id_(ps.get<size_t>("starting_fragment_id", 0)),
  nChannels_(ps.get<size_t>("nChannels", 600000)),
  adc_bits_ (ps.get<size_t>("adc_bits", 12)),
  run_number_(ps.get<RawDataType>("run_number")),
  engine_(ps.get<int64_t>("random_seed", 314159)),
  adc_freqs_(),
  content_generator_() {
  content_generator_.reserve(fragments_per_event_);
  read_adc_freqs(ps.get<std::string>("freqs_file"), adc_freqs_, adc_bits_);
  for (size_t i = 0; i < fragments_per_event_; ++i) {
    content_generator_.emplace_back(V172xFragment::adc_range(adc_bits_),
                                    -0.5,
                                    V172xFragment::adc_range(adc_bits_) - 0.5,
                                    [this, i](double x) -> double { return adc_freqs_[i][std::round(x)]; }
                                   );
  }
}

bool ds50::V172xSimulator::getNext_(FragmentPtrs & frags) {
  ++current_event_num_;
  if (events_to_generate_ != 0 &&
      current_event_num_ > events_to_generate_) {
    return false;
  }
  ds50::V172xFragment::Header::board_id_t fragID(starting_fragment_id_);
// #pragma omp parallel for shared(fragID, frags)
// TODO: Allow parallel operation by having multiple engines (with different seeds, of course).
  for (size_t i = 0; i < fragments_per_event_; ++i, ++fragID) {
    frags.emplace_back(new Fragment);
    V172xFragmentWriter newboard(*frags.back());
    newboard.resize(nChannels_);
    newboard.setBoardID(fragID);
    newboard.setEventCounter(current_event_num_);
    std::generate_n(newboard.dataBegin(),
                    nChannels_,
                    [this, i]() {
                      return static_cast<V172xFragment::adc_type>
                        (std::round(content_generator_[i](engine_)));
                    }
                   );
  }
  return true;
}

DEFINE_ARTDAQ_GENERATOR(ds50::V172xSimulator)
