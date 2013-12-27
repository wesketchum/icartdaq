#include "artdaq-demo/Generators/V172xSimulator.hh"

#include "art/Utilities/Exception.h"
#include "artdaq/Application/GeneratorMacros.hh"
#include "cetlib/exception.h"
#include "artdaq-demo/Overlays/V172xFragment.hh"
#include "artdaq-demo/Overlays/V172xFragmentWriter.hh"
#include "artdaq-demo/Overlays/FragmentType.hh"
#include "fhiclcpp/ParameterSet.h"
#include "artdaq/Utilities/SimpleLookupPolicy.h"

#include <fstream>
#include <iomanip>
#include <iterator>
#include <iostream>

#include <unistd.h>

namespace {
  size_t typeToADC(demo::FragmentType type)
  {
    switch (type) {
    case demo::FragmentType::V1720:
      return 12;
      break;
    case demo::FragmentType::V1724:
      return 14;
      break;
    default:
      throw art::Exception(art::errors::Configuration)
        << "Unknown board type "
        << type
        << " ("
        << demo::fragmentTypeToString(type)
        << ").\n";
    };
  }

  void read_adc_freqs(std::string const & fileName, std::string const & filePath,
                      std::vector<std::vector<size_t>> & freqs, int adc_bits) {

    // 06-Jan-2013, KAB - added the ability to find the specified data file
    // in a list of paths specified in an environmental variable
    if (getenv(filePath.c_str()) == nullptr) {
      setenv(filePath.c_str(), ".", 0);
    }
    artdaq::SimpleLookupPolicy lookup_policy(filePath);
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
      freq.resize(demo::V172xFragment::adc_range(adc_bits));
    }
    while (is.peek() != EOF) { // If we get EOF here, we're OK.
      if (!is.good()) {
        throw cet::exception("FileReadError")
          << "Error reading distribution data file "
          << fileName;
      }
      demo::V172xFragment::adc_type channel;
      is >> channel;
      for (auto & freq : freqs) {
        is >> freq[channel];
      }
      is.ignore(1);
    }
    is.close();
  }
}

demo::V172xSimulator::V172xSimulator(fhicl::ParameterSet const & ps)
  :
  CommandableFragmentGenerator(ps),
  nChannels_(ps.get<size_t>("nChannels", 600000)),
  fragment_type_(toFragmentType(ps.get<std::string>("fragment_type", "V1720"))),
  fragment_ids_(),
  current_event_num_(0ul),
  engine_(ps.get<int64_t>("random_seed", 314159)),
  adc_freqs_(),
  content_generator_()
{
  // Initialize fragment_ids_.
  fragment_ids_.resize(ps.get<size_t>("fragments_per_event", 5));
  auto current_id = ps.get<size_t>("starting_fragment_id", 0);
  std::generate(fragment_ids_.begin(),
                fragment_ids_.end(),
                [&current_id]() { return current_id++; });

  // Read frequency tables.
  size_t const adc_bits = typeToADC(fragment_type_);
  auto const fragments_per_event = fragment_ids_.size();
  content_generator_.reserve(fragments_per_event);
  read_adc_freqs(ps.get<std::string>("freqs_file"),
                 ps.get<std::string>("freqs_path", "DAQ_INDATA_PATH"),
                 adc_freqs_,
                 adc_bits);

  // Initialize content generators.
  for (size_t i = 0; i < fragments_per_event; ++i) {
    content_generator_.emplace_back(V172xFragment::adc_range(adc_bits),
                                    -0.5,
                                    V172xFragment::adc_range(adc_bits) - 0.5,
                                    [this, i](double x) -> double { return adc_freqs_[i][std::round(x)]; }
                                   );
  }
}

bool demo::V172xSimulator::getNext_(artdaq::FragmentPtrs & frags) {
  if (should_stop ()) {
    return false;
  }

  ++current_event_num_;

// #pragma omp parallel for shared(fragID, frags)
// TODO: Allow parallel operation by having multiple engines (with different seeds, of course).
  for (size_t i = 0; i < fragment_ids_.size(); ++i) {
    frags.emplace_back(new artdaq::Fragment);
    V172xFragmentWriter newboard(*frags.back());
    newboard.resize(nChannels_);
    newboard.setBoardID(fragment_ids_[i]);
    newboard.setEventCounter(current_event_num_);
    std::generate_n(newboard.dataBegin(),
                    nChannels_,
                    [this, i]() {
                      return static_cast<V172xFragment::adc_type>
                        (std::round(content_generator_[i](engine_)));
                    }
                   );

    artdaq::Fragment& frag = *frags.back();
    frag.setFragmentID (newboard.board_id());
    frag.setSequenceID (current_event_num_);
    frag.setUserType(fragment_type_);
  }

  return true;
}

std::vector<artdaq::Fragment::fragment_id_t>
demo::V172xSimulator::
fragmentIDs_()
{
  return fragment_ids_;
}
DEFINE_ARTDAQ_APPLICATION_GENERATOR(demo::V172xSimulator)
