#ifndef artdaq_demo_Overlays_V172xFragmentWriter_hh
#define artdaq_demo_Overlays_V172xFragmentWriter_hh
////////////////////////////////////////////////////////////////////////
// DS50V172xFragmentWriter
//
// Fragment overlay for writing DS50 boards.
//
////////////////////////////////////////////////////////////////////////

#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq-demo/Overlays/V172xFragment.hh"

namespace demo {
  class V172xFragmentWriter;
}

// Note the inheritance: order is important here (for construction
// initialization order).
class demo::V172xFragmentWriter: public demo::V172xFragment {
public:
  V172xFragmentWriter(artdaq::Fragment &);

  // These functions form overload sets with const functions from
  // demo::Board.
  adc_type * dataBegin();
  adc_type * dataEnd();

// TODO: Render consistent with naming for accessors, one way or the
// other. Note Fragment getters / setters should be taken into account
// also.
  void setChannelMask(demo::V172xFragment::Header::channel_mask_t mask);
  void setPattern(demo::V172xFragment::Header::pattern_t pattern);
  void setBoardID(demo::V172xFragment::Header::board_id_t id);
  void setEventCounter(demo::V172xFragment::Header::event_counter_t event_counter);
  void setTriggerTimeTag(demo::V172xFragment::Header::trigger_time_tag_t tag);

  void resize(size_t nAdcs);

private:
  size_t calc_event_size_words_(size_t nAdcs);

  static size_t adcs_to_words_(size_t nAdcs);

  demo::V172xFragment::Header * header_();

  artdaq::Fragment & frag_;
};

inline demo::V172xFragmentWriter::V172xFragmentWriter(artdaq::Fragment & frag): V172xFragment(frag), frag_(frag) { }

inline demo::V172xFragment::adc_type * demo::V172xFragmentWriter::dataBegin() {
  // Make sure there's data past the V172xFragment header
  assert(frag_.dataSizeBytes() >= sizeof(Header) + sizeof(artdaq::Fragment::value_type) );
  return reinterpret_cast<adc_type *>(header_() + 1);
}

inline demo::V172xFragment::adc_type * demo::V172xFragmentWriter::dataEnd() {
  return dataBegin() + total_adc_values();
}

inline void demo::V172xFragmentWriter::setChannelMask(demo::V172xFragment::Header::channel_mask_t mask) {
  header_()->channel_mask = mask;
}

inline void demo::V172xFragmentWriter::setPattern(demo::V172xFragment::Header::pattern_t pattern) {
  header_()->pattern = pattern;
}

inline void demo::V172xFragmentWriter::setBoardID(demo::V172xFragment::Header::board_id_t id) {
  header_()->board_id = id;
  frag_.setFragmentID(id);
}

inline void demo::V172xFragmentWriter::setEventCounter(demo::V172xFragment::Header::event_counter_t event_counter) {
  header_()-> event_counter = event_counter;
  frag_.setSequenceID(event_counter);
}

inline void demo::V172xFragmentWriter::setTriggerTimeTag(demo::V172xFragment::Header::trigger_time_tag_t tag) {
  header_()->trigger_time_tag = tag;
}

inline void demo::V172xFragmentWriter::resize(size_t nAdcs) {

  frag_.resizeBytes( sizeof(Header::data_t) * calc_event_size_words_(nAdcs) );
  header_()->event_size = calc_event_size_words_(nAdcs);
}

inline size_t demo::V172xFragmentWriter::calc_event_size_words_(size_t nAdcs) {
  return adcs_to_words_(nAdcs) + header_size_words();
}

inline size_t demo::V172xFragmentWriter::adcs_to_words_(size_t nAdcs) {
  auto mod(nAdcs % adcs_per_word_());
  return (mod == 0) ?
    nAdcs / adcs_per_word_() :
    nAdcs / adcs_per_word_() + 1;
}

inline demo::V172xFragment::Header * demo::V172xFragmentWriter::header_() {
  assert(frag_.dataSizeBytes() >= sizeof(Header));
  return reinterpret_cast<demo::V172xFragment::Header *>(frag_.dataBeginBytes());
}

#endif /* artdaq_demo_Overlays_V172xFragmentWriter_hh */
