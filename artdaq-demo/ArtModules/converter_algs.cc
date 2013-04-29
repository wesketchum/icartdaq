#include "darkart/ArtModules/converter_algs.hh"
#include "ds50daq/DAQ/Config.hh"
#include "art/Utilities/Exception.h"

double darkart::scaling_factor(artdaq::Fragment::type_t board_type)
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

void darkart::convert_channel(ds50::V172xFragment::adc_type const * begin,
                              ds50::V172xFragment::adc_type const * end,
                              darkart::Channel& output,
                              artdaq::Fragment::type_t board_type)
{
  double const scale = scaling_factor(board_type);    
  for ( ; begin != end; ++begin) output.waveform.push_back(*begin * scale);
}

void darkart::convert_fragment(artdaq::Fragment const& frag,
                               darkart::Channels& output,
                               artdaq::Fragment::type_t board_type)
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

void darkart::convert_fragments(artdaq::Fragments const& input,
                                darkart::Channels& output,
                                artdaq::Fragment::type_t board_type)
{
  for (auto const& frag : input)
    convert_fragment(frag, output, board_type);
}
