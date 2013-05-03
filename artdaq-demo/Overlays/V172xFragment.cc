#include "artdaq-demo/Overlays/V172xFragment.hh"

#include "cetlib/exception.h"

namespace {
  unsigned int pop_count (unsigned int n) {
    unsigned int c; 
    for (c = 0; n; c++) n &= n - 1; 
    return c;
  }
}

void demo::V172xFragment::checkADCData(int daq_adc_bits) const {
  demo::V172xFragment::adc_type const * adcPtr(findBadADC(daq_adc_bits));
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

bool demo::V172xFragment::channel_present(int ch) const { 
  if (ch < 0 || ch > 7) throw cet::exception("IllegalChannel") << "requested ch #" << ch << ",";
  return header_()->channel_mask && (1 << ch); 
}

unsigned int demo::V172xFragment::enabled_channels() const {
  return pop_count (channel_mask ());
}

demo::V172xFragment::adc_type const * demo::V172xFragment::chDataBegin(int ch) const {
  if (!channel_present(ch)) throw cet::exception("DataNotPresent") << "requested missing ch #" << ch << ",";
  int ordinal = pop_count (channel_mask () & ((2 << ch) - 1)) - 1;
  return dataBegin () + ordinal * adc_values_for_channel ();
}

std::ostream & demo::operator << (std::ostream & os, V172xFragment const & b) {
  os << "V172xFragment ID: "
     << static_cast<uint16_t>(b.board_id())
     << ", event counter: "
     << b.event_counter()
     << ". event size: "
     << b.event_size()
     << "w, channels: "
     << b.total_adc_values()
     << ", trigger time: "
     << b.trigger_time_tag()
     << "\n";
  os << "  Channel mask: "
     << std::hex
     << static_cast<uint16_t>(b.channel_mask())
     << std::dec
     << ", pattern: "
     << b.pattern()
     << ".\n";
  return os;
}

