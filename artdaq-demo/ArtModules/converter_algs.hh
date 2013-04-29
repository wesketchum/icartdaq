#ifndef artdaq_demo_ArtModules_converter_algs_hh
#define artdaq_demo_ArtModules_converter_algs_hh

#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq/DAQdata/Fragments.hh"
#include "artdaq-demo/Products/Channel.hh"
#include "artdaq-demo/Overlays/V172xFragment.hh"

namespace darkart
{
  // Determine the 'scaling factor' for the given board type. We are
  // assuming that different board types require different scaling
  // factors when converting from integral ADC counts to
  // floating-point signal measurements. THIS IS A TOY and should be
  // replaced by a real algorithm!
  double scaling_factor(artdaq::Fragment::type_t board_type);

  // Convert the data for one channel held in a Fragment (as integral
  // raw ADC counts) to the values stored as floating-point
  // numbers. This is the point that calibration might be applied, if
  // such calibration is appropriate. This implementation uses the toy
  // function 'scaling_factor'.
  void convert_channel(demo::V172xFragment::adc_type const * begin,
                       demo::V172xFragment::adc_type const * end,
                       Channel& output,
                       artdaq::Fragment::type_t board_type);

  // Translate a single artdaq::Fragment object into a series of
  // darkart::Channel objects.
  void convert_fragment(artdaq::Fragment const& frag,
                        Channels& output,
                        artdaq::Fragment::type_t board_type);

  // Loop over all the artdaq::Fragments in 'input' and create from
  // each the appropriate darkart::Channels objects.
  void convert_fragments(artdaq::Fragments const& input,
                         Channels& output,
                         artdaq::Fragment::type_t board_type);

  // Translate a single channel from raw ADC counts to a
  // darkart::Channel.
  void convert_channel(demo::V172xFragment::adc_type const * begin,
                       demo::V172xFragment::adc_type const * end,
                       darkart::Channel& output,
                       artdaq::Fragment::type_t board_type);
}

#endif /* artdaq_demo_ArtModules_converter_algs_hh */
