#include "artdaq/DAQdata/Fragment.hh"
#include "ds50daq/DAQ/CompressedV172x.hh"
#include "art/Persistency/Common/Wrapper.h"
#include <vector>

template class std::vector<artdaq::Fragment>;
template class art::Wrapper<std::vector<artdaq::Fragment> >;

namespace {
  struct dictionary {
    ds50::CompressedV172x d1;
  };
}

template class art::Wrapper<ds50::CompressedV172x>;
