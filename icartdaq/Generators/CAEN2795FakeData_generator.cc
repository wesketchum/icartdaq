#include "icartdaq/Generators/CAEN2795FakeData.hh"

#include "art/Utilities/Exception.h"
#include "artdaq/Application/GeneratorMacros.hh"
#include "cetlib/exception.h"
#include "icartdaq-core/Overlays/CAEN2795Fragment.hh"
#include "icartdaq-core/Overlays/CAEN2795FragmentWriter.hh"
#include "icartdaq-core/Overlays/FragmentType.hh"
#include "fhiclcpp/ParameterSet.h"
#include "artdaq-core/Utilities/SimpleLookupPolicy.h"

#include <fstream>
#include <iomanip>
#include <iterator>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


demo::CAEN2795FakeData::CAEN2795FakeData(fhicl::ParameterSet const & ps)
  :
  CommandableFragmentGenerator(ps),
  nSamplesPerChannel_(ps.get<uint32_t>("nSamplesPerChannel",3000)),
  nADCBits_(ps.get<uint8_t>("nADCBits",12)),
  nChannelsPerBoard_(ps.get<uint16_t>("nChannelsPerBoard",64)),
  nBoards_(ps.get<uint16_t>("nBoards",9)),
  RunNumber_(ps.get<uint32_t>("RunNumber",999)),
  throttle_usecs_(ps.get<size_t>("throttle_usecs", 100000)),
  throttle_usecs_check_(ps.get<size_t>("throttle_usecs_check", 10000)),
  engine_(ps.get<int64_t>("random_seed", 314159)),
  uniform_distn_(new std::uniform_int_distribution<int>(0, pow(2,nADCBits_) - 1 ))
{

  if (throttle_usecs_ > 0 && (throttle_usecs_check_ >= throttle_usecs_ ||
			      throttle_usecs_ % throttle_usecs_check_ != 0) ) {
    throw cet::exception("Error in CAEN2795: disallowed combination of throttle_usecs and throttle_usecs_check (see CAEN2795.hh for rules)");
  }
    
}

bool demo::CAEN2795FakeData::getNext_(artdaq::FragmentPtrs & frags) {

  // JCF, 9/23/14

  // If throttle_usecs_ is greater than zero (i.e., user requests a
  // sleep interval before generating the pseudodata) then during that
  // interval perform a periodic check to see whether a stop request
  // has been received

  // Values for throttle_usecs_ and throttle_usecs_check_ will have
  // been tested for validity in constructor

  if (throttle_usecs_ > 0) {
    size_t nchecks = throttle_usecs_ / throttle_usecs_check_;

    for (size_t i_c = 0; i_c < nchecks; ++i_c) {
      usleep( throttle_usecs_check_ );

      if (should_stop()) {
	return false;
      }
    }
  } else {
    if (should_stop()) {
      return false;
    }
  }

  // Set fragment's metadata

  CAEN2795Fragment::Metadata metadata;
  metadata.samples_per_channel = nSamplesPerChannel_;
  metadata.num_adc_bits = nADCBits_;
  metadata.channels_per_board = nChannelsPerBoard_;
  metadata.num_boards = nBoards_;

  // And use it, along with the artdaq::Fragment header information
  // (fragment id, sequence id, and user type) to create a fragment

  // We'll use the static factory function 

  // artdaq::Fragment::FragmentBytes(std::size_t payload_size_in_bytes, sequence_id_t sequence_id,
  //  fragment_id_t fragment_id, type_t type, const T & metadata)

  // which will then return a unique_ptr to an artdaq::Fragment
  // object. The advantage of this approach over using the
  // artdaq::Fragment constructor is that, if we were to want to
  // initialize the artdaq::Fragment with a nonzero-size payload (data
  // after the artdaq::Fragment header and metadata), we could provide
  // the size of the payload in bytes, rather than in units of the
  // artdaq::Fragment's RawDataType (8 bytes, as of 3/26/14). The
  // artdaq::Fragment constructor itself was not altered so as to
  // maintain backward compatibility.

  std::size_t initial_payload_size = 0;

  frags.emplace_back( artdaq::Fragment::FragmentBytes(initial_payload_size,  
						      ev_counter(), fragment_id(),
						      demo::detail::FragmentType::CAEN2795, metadata) );

  std::cout << "Initialized data of size " << frags.back()->dataSize() << std::endl;

  // Then any overlay-specific quantities next; will need the
  // ToyFragmentWriter class's setter-functions for this

  CAEN2795FragmentWriter newfrag(*frags.back());

  newfrag.set_hdr_run_number(RunNumber_);

  newfrag.resize(nSamplesPerChannel_*nChannelsPerBoard_*nBoards_ + 4*nBoards_);

  // And generate nADCcounts ADC values ranging from 0 to max with an
  // equal probability over the full range (a specific and perhaps
  // not-too-physical example of how one could generate simulated
  // data)

  uint32_t time_stamp = ev_counter() + 100;

  for(size_t i_b=0; i_b<nBoards_; i_b++){
    newfrag.CAEN2795_hdr(i_b)->ev_num = (ev_counter() & 0xffffff);
    newfrag.CAEN2795_hdr(i_b)->unused1 = 0;
    newfrag.CAEN2795_hdr(i_b)->time_st = time_stamp;
    std::generate_n(newfrag.dataBegin(i_b), nSamplesPerChannel_*nChannelsPerBoard_,
		    [&]() {
		      return static_cast<CAEN2795Fragment::adc_t>
			((*uniform_distn_)( engine_ ));
		    }
		    );
  }

  std::cout << "Sending data of size " << frags.back()->dataSize() << std::endl;

  if(metricMan_ != nullptr) {
    metricMan_->sendMetric("Fragments Sent",ev_counter(), "Events", 3);
  }
  // Check and make sure that no ADC values in this fragment are
  // larger than the max allowed

  //newfrag.fastVerify( metadata.num_adc_bits );

  ev_counter_inc();

  return true;
}

// The following macro is defined in artdaq's GeneratorMacros.hh header
DEFINE_ARTDAQ_COMMANDABLE_GENERATOR(demo::CAEN2795FakeData) 
