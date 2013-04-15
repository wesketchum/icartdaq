#include "ds50daq/DAQ/V172xSimulator.hh"

#include "artdaq/DAQdata/GeneratorMacros.hh"
#include "cetlib/exception.h"
#include "ds50daq/DAQ/Config.hh"
#include "ds50daq/DAQ/V172xFragment.hh"
#include "ds50daq/DAQ/V172xFragmentWriter.hh"
#include "fhiclcpp/ParameterSet.h"
#include "artdaq/Utilities/SimpleLookupPolicy.h"

#include <fstream>
#include <iomanip>
#include <iterator>
#include <iostream>

#include <unistd.h>

using namespace artdaq;

namespace {
  void read_adc_freqs(std::string const & fileName, std::string const & filePath,
                      std::vector<std::vector<size_t>> & freqs, int adc_bits) {

    // 06-Jan-2013, KAB - added the ability to find the specified data file
    // in a list of paths specified in an environmental variable
    if (getenv(filePath.c_str()) == nullptr) {
      setenv(filePath.c_str(), ".", 0);
    }
    SimpleLookupPolicy lookup_policy(filePath);
    std::string fullPath = fileName;
    try {fullPath = lookup_policy(fileName);}
    catch (...) {}

    std::ifstream is(fullPath);
    if (!is) {
      throw cet::exception("FileOpenError")
        << "Unable to open distribution data file "
        << fileName << " with paths in " << filePath << ".";
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
  DS50FragmentGenerator(ps.get<fhicl::ParameterSet> ("generator_ds50")),
  current_event_num_(1),
  fragments_per_event_(ps.get<size_t>("fragments_per_event", 5)),
  starting_fragment_id_(ps.get<size_t>("starting_fragment_id", 0)),
  nChannels_(ps.get<size_t>("nChannels", 600000)),
  adc_bits_ (ps.get<size_t>("adc_bits", 12)),
  engine_(ps.get<int64_t>("random_seed", 314159)),
  adc_freqs_(),
  content_generator_() {
  content_generator_.reserve(fragments_per_event_);
  read_adc_freqs(ps.get<std::string>("freqs_file"),
                 ps.get<std::string>("freqs_path", "DS50DAQ_CONFIG_PATH"),
                 adc_freqs_, adc_bits_);
  for (size_t i = 0; i < fragments_per_event_; ++i) {
    content_generator_.emplace_back(V172xFragment::adc_range(adc_bits_),
                                    -0.5,
                                    V172xFragment::adc_range(adc_bits_) - 0.5,
                                    [this, i](double x) -> double { return adc_freqs_[i][std::round(x)]; }
                                   );
  }
}

bool ds50::V172xSimulator::getNext__(FragmentPtrs & frags) {
  if (should_stop ()) {
    std::cout << "ds50::V172xSimulator::getNext__(): Sending EOD Fragment." << std::endl;
    frags.emplace_back(artdaq::Fragment::eodFrag(1));
    return false;
  }

  ++current_event_num_;

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

    artdaq::Fragment& frag = *frags.back();
    frag.setFragmentID (fragment_id ());
    frag.setSequenceID (current_event_num_);
    frag.setUserType (Config::V1720_FRAGMENT_TYPE);
  }

  usleep(500000);
  return true;
}

DEFINE_ARTDAQ_GENERATOR(ds50::V172xSimulator)
