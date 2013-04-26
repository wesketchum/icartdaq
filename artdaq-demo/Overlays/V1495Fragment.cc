#include "artdaq-demo/Overlays/V1495Fragment.hh"
#include "cetlib/exception.h"

bool demo::V1495Fragment::channel_in_trigger (int ch) const {
  if (ch < 0 || ch >= 38) throw cet::exception("V1495Fragment") << "illegal channel request " << ch;
  return (trigger_pattern () >> ch) & 1;
  //if (ch < 15) return (trigger_pattern () >> ch) & 1;
  //else if (ch < 30) return (trigger_pattern () >> ++ch) & 1;
  //else return (trigger_pattern () >> ++ch) & 1;
}

std::ostream & demo::operator << (std::ostream & os, V1495Fragment const & b) {
  os << "V1495 Fragment Run: "
    << b.run_number ()
    << ", event counter: "
    << b.trigger_counter ()
    << ", event type: "
    << static_cast<int>(b.trigger_type ())
    << ", trigger pattern "
    << b.trigger_pattern ()
    << ". event size: "
    << b.event_size ();

  return os;
}

