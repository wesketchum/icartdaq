#include "artdaq-demo/Overlays/V1190Fragment.hh"

std::ostream & demo::operator << (std::ostream &o, demo::V1190Fragment const &f) {
  for (demo::V1190Fragment::Word const * it = f.dataBegin (); it != f.dataEnd (); it++) o << *it << " -" <<  std::endl;
  return o;
}
std::ostream & demo::operator << (std::ostream &o, demo::V1190Fragment::Word const &w) {
  o << std::hex << std::showbase;
  switch (w.type ()) {
    case demo::V1190Fragment::Word::global_header:
      o << "global header ev_count " << w.event_count () << " geo " << int (w.geo ());
      break;
    case demo::V1190Fragment::Word::tdc_header:
      o << "tdc header tdc " << int(w.tdc ()) << " ev_id " << w.event_id () << " bunch_id " << w.bunch_id ();
      break;
    case demo::V1190Fragment::Word::measurement_w:
      o << "tdc header " << (w.trailing_edge () ? "trailing" : "leading") << " channel " << std::dec << int(w.channel ()) << " measurement " << w.measurement ();
      break;
    case demo::V1190Fragment::Word::tdc_trailer:
      o << "tdc trailer tdc " << int(w.tdc ()) << " ev_id " << w.event_id () << " word_count " << w.tdc_word_count ();
      break;
    case demo::V1190Fragment::Word::tdc_error:
      o << "tdc error tdc " << int(w.tdc ()) << " error_flags " << w.error_flags ();
      break;
    case demo::V1190Fragment::Word::extended_time_tag_w:
      o << "extended time tag " << w.extended_time_tag ();
      break;
    case demo::V1190Fragment::Word::trailer:
      o << "global trailer status " << int(w.status ()) << " word_count " << w.word_count () << " geo " << int(w.geo ());
      break;
    case demo::V1190Fragment::Word::filler:
      o << "filler";
      break;
  };
  o << std::dec;

  return o;
}

