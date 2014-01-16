#include "artdaq-demo/Overlays/ToyFragment.hh"

#include "cetlib/exception.h"

namespace {
  unsigned int pop_count (unsigned int n) {
    unsigned int c; 
    for (c = 0; n; c++) n &= n - 1; 
    return c;
  }
}

void demo::ToyFragment::checkADCData(int daq_adc_bits) const {
  demo::ToyFragment::adc_t const * adcPtr(findBadADC(daq_adc_bits));
  if (adcPtr != dataEnd()) {
    throw cet::exception("IllegalADCVal")
        << "Illegal value of ADC word #"
        << (adcPtr - dataBegin())
        << ": 0x"
        << std::hex
        << *adcPtr
        << ".";
  }
}

// bool demo::ToyFragment::channel_present(int ch) const { 
//   if (ch < 0 || ch > 7) throw cet::exception("IllegalChannel") << "requested ch #" << ch << ",";

//   return header_()->channel_mask & (1 << ch); 

// }

// unsigned int demo::ToyFragment::enabled_channels() const {
//   return pop_count (channel_mask ());
// }

// demo::ToyFragment::adc_t const * demo::ToyFragment::chDataBegin(int ch) const {
//   if (!channel_present(ch)) throw cet::exception("DataNotPresent") << "requested missing ch #" << ch << ",";
//   int ordinal = pop_count (channel_mask () & ((2 << ch) - 1)) - 1;
//   return dataBegin () + ordinal * adc_values_for_channel ();
// }


std::ostream & demo::operator << (std::ostream & os, ToyFragment const & b) {
  os << "ToyFragment event size: "
     << b.hdr_event_size()
     << ", run number: "
     << b.hdr_run_number()
     << ". event number: "
     << b.hdr_event_number()
     << "\n";
  //  os << "  Channel mask: "
  //     << std::hex
  //     << static_cast<uint16_t>(b.channel_mask())
  //     << std::dec
  //     << ", pattern: "
  //     << b.pattern()
  //     << ".\n";
  return os;
}

