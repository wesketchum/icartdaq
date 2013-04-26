#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq-demo/Products/CompressedV172x.hh"
#include "art/Persistency/Common/Wrapper.h"
#include <vector>

template class std::vector<artdaq::Fragment>;
template class art::Wrapper<std::vector<artdaq::Fragment> >;

namespace {
  struct dictionary {
    demo::CompressedV172x d1;
  };
}

template class art::Wrapper<demo::CompressedV172x>;
