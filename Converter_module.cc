////////////////////////////////////////////////////////////////////////
// Class:       Converter
// Module Type: producer
// File:        Converter_module.cc
//
// Generated at Thu Mar 21 20:32:50 2013 by James Kowalkowski using artmod
// from art v1_03_03.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Utilities/Exception.h"

#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq/DAQdata/Fragments.hh"

#include "ds50daq/DAQ/V172xFragment.hh"
#include "ds50daq/DAQ/Config.hh"

#include "darkart/Products/Channel.hh"

#include <utility>

class Converter;

class Converter : public art::EDProducer {
public:
  explicit Converter(fhicl::ParameterSet const & p);
  virtual ~Converter();

  void produce(art::Event & e) override;
};

//----------------------------------------------------------------------------
// We begin with some helper functions, to be used in the implementation
// of Converter. These could be moved to a separate library (not module)
// if they become of interest to other modules. They are not members of
// Converter because they do not need to be, and making them non-members
// makes modifcation of the code easier.

// Determine the 'scaling factor' for the given board type. We are
// assuming that different board types require different scaling factors
// when converting from integral ADC counts to floating-point signal
// measurements.
double
scaling_factor(artdaq::detail::RawFragmentHeader::type_t board_type)
{
  switch (board_type)
    {
    case ds50::Config::V1720_FRAGMENT_TYPE:
      return 1.0;
    case ds50::Config::V1724_FRAGMENT_TYPE:
      return 0.1;
    }
  throw art::Exception(art::errors::DataCorruption)
    << "Board type '" << board_type << "' not recognized in scaling_factor";
}

// Convert the data for one channel held in a Fragment (as integral raw
// ADC counts) to the values stored as floating-point numbers. This is
// the point that calibration might be applied, if such calibration is
// appropriate. In this initial implementation, we do no such
// calibration.
void convert_channel(ds50::V172xFragment::adc_type const * begin,
                     ds50::V172xFragment::adc_type const * end,
                     darkart::Channel& output,
                     artdaq::detail::RawFragmentHeader::type_t board_type)
{
  double const scale = scaling_factor(board_type);    
  for ( ; begin != end; ++begin) output.waveform.push_back(*begin * scale);
}


// This is the helper function that encapsulates the algorithm of
// translating a single artdaq::Fragment object into series of
// darkart::Channel objects.
void convert_fragment(artdaq::Fragment const& frag,
                      darkart::Channels& output,
                      artdaq::detail::RawFragmentHeader::type_t board_type)
{
  // Use the overlay to allow us to read channels.
  ds50::V172xFragment overlay(frag);

  // Loop over all channels in the fragment, and make a Channel object
  // for each one.
  for (size_t i = 0, nchan = overlay.enabled_channels(); i != nchan; ++i)
    {
      output.emplace_back(overlay.board_id(), i,
                          overlay.adc_values_for_channel());
      convert_channel(overlay.chDataBegin(i), 
                      overlay.chDataEnd(i),
                      output.back(),
                      board_type);
    }
}

// This is the helper function that loops over all the artdaq::Fragments
// in its input, and creates from each darkart::Channels objects.
void convert_fragments(artdaq::Fragments const& input,
                       darkart::Channels& output,
                       artdaq::detail::RawFragmentHeader::type_t board_type)
{
  for (auto const& frag : input)
    convert_fragment(frag, output, board_type);
}


//----------------------------------------------------------------------------
// The implementation of the module member functions begins here.

Converter::Converter(fhicl::ParameterSet const & /* p */)
{
  produces<darkart::Channels>("V1720");
  produces<darkart::Channels>("V1724");
}

Converter::~Converter()
{ }

void Converter::produce(art::Event & e)
{
  // need to get module_label="daq" and instance_name="1720" or "1724"
  art::Handle<artdaq::Fragments> h_1720, h_1724;
  e.getByLabel("daq", "V1720", h_1720);
  e.getByLabel("daq", "V1724", h_1724);

  // Make our products, which begin empty.
  std::unique_ptr<darkart::Channels> v1720(new darkart::Channels);
  std::unique_ptr<darkart::Channels> v1724(new darkart::Channels);

  // Call the algorithm that will fill the products. We have to pass in
  // the enumeration value that tells us the type of board we're dealing
  // with, because the data don't carry that information directly.
  convert_fragments(*h_1720, *v1720, ds50::Config::V1720_FRAGMENT_TYPE);
  convert_fragments(*h_1724, *v1724, ds50::Config::V1724_FRAGMENT_TYPE);

  // Put our complete products into the Event.
  e.put(std::move(v1720));
  e.put(std::move(v1724));
}


DEFINE_ART_MODULE(Converter)
