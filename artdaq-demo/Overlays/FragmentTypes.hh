#ifndef artdaq_demo_Overlays_FragmentType_hh
#define artdaq_demo_Overlays_FragmentType_hh
#include "artdaq/DAQdata/Fragment.hh"

namespace demo {

  namespace detail {
    enum FragmentTypes : artdaq::Fragment::type_t
    { MISSED = artdaq::Fragment::FirstUserFragmentType,
        V1495,
        V1720,
        V1724,
        V1190
        };

    // Safety check.
    static_assert(artdaq::Fragment::isUserFragmentType(FragmentTypes::V1190),
                  "Too many user-defined fragments!");
  }

  using detail::FragmentTypes;

}
#endif /* artdaq_demo_Overlays_FragmentType_hh */
