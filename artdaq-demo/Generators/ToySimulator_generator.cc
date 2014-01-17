#include "artdaq-demo/Generators/ToySimulator.hh"

#include "art/Utilities/Exception.h"
#include "artdaq/Application/GeneratorMacros.hh"
#include "cetlib/exception.h"
#include "artdaq-demo/Overlays/ToyFragment.hh"
#include "artdaq-demo/Overlays/ToyFragmentWriter.hh"
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
    case demo::FragmentType::TOY1:
      return 12;
      break;
    case demo::FragmentType::TOY2:
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

  // void read_adc_freqs(std::string const & fileName, std::string const & filePath,
  //                     std::vector<std::vector<size_t>> & freqs, int adc_bits) {

  //   // 06-Jan-2013, KAB - added the ability to find the specified data file
  //   // in a list of paths specified in an environmental variable
  //   if (getenv(filePath.c_str()) == nullptr) {
  //     setenv(filePath.c_str(), ".", 0);
  //   }
  //   artdaq::SimpleLookupPolicy lookup_policy(filePath);
  //   std::string fullPath = fileName;
  //   try {fullPath = lookup_policy(fileName);}
  //   catch (...) {}

  //   std::ifstream is(fullPath);
  //   if (!is) {
  //     throw cet::exception("FileOpenError")
  //       << "Unable to open distribution data file "
  //       << fileName << " with paths in " << filePath << ".";
  //   }
  //   std::string header;
  //   std::getline(is, header);
  //   is.ignore(1);
  //   std::vector<std::string> split_headers;
  //   cet::split(header, ' ', std::back_inserter(split_headers));
  //   size_t nHeaders = split_headers.size();
  //   freqs.clear();
  //   freqs.resize(nHeaders -1); // Take account of ADC column.
  //   for (auto & freq : freqs) {
  //     freq.resize(demo::ToyFragment::adc_range(adc_bits));
  //   }
  //   while (is.peek() != EOF) { // If we get EOF here, we're OK.
  //     if (!is.good()) {
  //       throw cet::exception("FileReadError")
  //         << "Error reading distribution data file "
  //         << fileName;
  //     }
  //     demo::ToyFragment::adc_type channel;
  //     is >> channel;
  //     for (auto & freq : freqs) {
  //       is >> freq[channel];
  //     }
  //     is.ignore(1);
  //   }
  //   is.close();
  // }
}



demo::ToySimulator::ToySimulator(fhicl::ParameterSet const & ps)
  :
  CommandableFragmentGenerator(ps),
  nADCcounts_(ps.get<size_t>("nADCcounts", 600000)),
  fragment_type_(toFragmentType(ps.get<std::string>("fragment_type", "TOY1"))),
  fragment_ids_(1),
  current_event_num_(0ul),
  //  adc_freqs_(),
  //  content_generator_(),
  should_stop_(false),
  engine_(ps.get<int64_t>("random_seed", 314159)),
  uniform_distn_(new std::uniform_int_distribution<int>(0, pow(2, typeToADC( fragment_type_ ) ) - 1 ))
{

  // Check and make sure that the fragment type will be one of the "toy" types
  
  std::vector<artdaq::Fragment::type_t> const ftypes = {demo::FragmentType::TOY1, demo::FragmentType::TOY2 };

  if (std::find( ftypes.begin(), ftypes.end(), fragment_type_) == ftypes.end() ) {
    throw cet::exception("Error in ToySimulator: unexpected fragment type supplied to constructor");
  }

  auto current_id = ps.get<size_t>("starting_fragment_id", 0);

  // This functionality's included should a future developer want to include more than one fragment ID

  std::generate(fragment_ids_.begin(),
                fragment_ids_.end(),
                [&current_id]() { return current_id++; });

  // // Read frequency tables.
  // size_t const adc_bits = typeToADC(fragment_type_);
  // auto const fragments_per_board = fragment_ids_.size();
  // content_generator_.reserve(fragments_per_board);
  // read_adc_freqs(ps.get<std::string>("freqs_file"),
  //                ps.get<std::string>("freqs_path", "DAQ_INDATA_PATH"),
  //                adc_freqs_,
  //                adc_bits);

  // // Initialize content generators and set up separate random # generators for each fragment on the board
  // for (size_t i = 0; i < fragments_per_board; ++i) {
  //   content_generator_.emplace_back(ToyFragment::adc_range(adc_bits),
  //                                   -0.5,
  //                                   ToyFragment::adc_range(adc_bits) - 0.5,
  //                                   [this, i](double x) -> double { return adc_freqs_[i][std::round(x)]; }
  //                                  );
  // }
    
}


bool demo::ToySimulator::getNext_(artdaq::FragmentPtrs & frags) {

  if (should_stop()) {
    return false;
  }

  ++current_event_num_;

  //  for (size_t i = 0; i < fragment_ids_.size(); ++i) {
    
  frags.emplace_back(new artdaq::Fragment);

  // Set the artdaq::Fragment header quantities first

  artdaq::Fragment& frag = *frags.back();

  frag.setFragmentID (fragment_ids_[0]); 
  frag.setSequenceID (current_event_num_);
  frag.setUserType(fragment_type_);
  frag.setVersion(0);

  // Then the metadata

  ToyFragmentWriter::Metadata metadata;
  metadata.board_serial_number = 999;
  metadata.num_adc_bits = typeToADC(fragment_type_);
  frag.setMetadata( metadata );

  // Then any overlay-specific quantities next; will need the *Writer class for this

  ToyFragmentWriter newfrag(*frags.back());

  newfrag.set_hdr_run_number(999);
  newfrag.set_hdr_event_number(current_event_num_);
  
  newfrag.resize(nADCcounts_);

  std::generate_n(newfrag.dataBegin(), nADCcounts_,
  		  [&]() {
  		    return static_cast<ToyFragment::adc_t>
  		      ((*uniform_distn_)( engine_ ));
  		  }
  		  );

  newfrag.fastVerify( metadata.num_adc_bits );

  return true;
}

std::vector<artdaq::Fragment::fragment_id_t>
demo::ToySimulator::
fragmentIDs_()
{
  return fragment_ids_;
}
DEFINE_ARTDAQ_COMMANDABLE_GENERATOR(demo::ToySimulator)
