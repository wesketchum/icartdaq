#ifndef ds50daq_DAQ_V172xFragmentWriter_hh
#define ds50daq_DAQ_V172xFragmentWriter_hh
////////////////////////////////////////////////////////////////////////
// DS50V172xFragmentWriter
//
// Fragment overlay for writing DS50 boards.
//
////////////////////////////////////////////////////////////////////////

#include "artdaq/DAQdata/Fragment.hh"
#include "ds50daq/DAQ/V172xFragment.hh"

namespace ds50 {
  class V172xFragmentWriter;
}

// Note the inheritance: order is important here (for construction
// initialization order).
class ds50::V172xFragmentWriter: public ds50::V172xFragment {
public:
  V172xFragmentWriter(artdaq::Fragment &);

  // These functions form overload sets with const functions from
  // ds50::Board.
  adc_type * dataBegin();
  adc_type * dataEnd();

// TODO: Render consistent with naming for accessors, one way or the
// other. Note Fragment getters / setters should be taken into account
// also.
  void setChannelMask(ds50::V172xFragment::Header::channel_mask_t mask);
  void setPattern(ds50::V172xFragment::Header::pattern_t pattern);
  void setBoardID(ds50::V172xFragment::Header::board_id_t id);
  void setEventCounter(ds50::V172xFragment::Header::event_counter_t event_counter);
  void setTriggerTimeTag(ds50::V172xFragment::Header::trigger_time_tag_t tag);

  void resize(size_t nAdcs);

private:
  size_t calc_event_size_words_(size_t nAdcs);

  static size_t adcs_to_words_(size_t nAdcs);
  static size_t words_to_frag_words_(size_t nWords);

  ds50::V172xFragment::Header * header_();

  artdaq::Fragment & frag_;
};

inline ds50::V172xFragmentWriter::V172xFragmentWriter(artdaq::Fragment & frag): V172xFragment(frag), frag_(frag) { }

inline ds50::V172xFragment::adc_type * ds50::V172xFragmentWriter::dataBegin() {
  assert(frag_.dataSize() > words_to_frag_words_(header_size_words()));
  return reinterpret_cast<adc_type *>(header_() + 1);
}

inline ds50::V172xFragment::adc_type * ds50::V172xFragmentWriter::dataEnd() {
  return dataBegin() + total_adc_values();
}

inline void ds50::V172xFragmentWriter::setChannelMask(ds50::V172xFragment::Header::channel_mask_t mask) {
  header_()->channel_mask = mask;
}

inline void ds50::V172xFragmentWriter::setPattern(ds50::V172xFragment::Header::pattern_t pattern) {
  header_()->pattern = pattern;
}

inline void ds50::V172xFragmentWriter::setBoardID(ds50::V172xFragment::Header::board_id_t id) {
  header_()->board_id = id;
  frag_.setFragmentID(id);
}

inline void ds50::V172xFragmentWriter::setEventCounter(ds50::V172xFragment::Header::event_counter_t event_counter) {
  header_()-> event_counter = event_counter;
  frag_.setSequenceID(event_counter);
}

inline void ds50::V172xFragmentWriter::setTriggerTimeTag(ds50::V172xFragment::Header::trigger_time_tag_t tag) {
  header_()->trigger_time_tag = tag;
}

inline void ds50::V172xFragmentWriter::resize(size_t nAdcs) {
  auto es(calc_event_size_words_(nAdcs));
  frag_.resize(words_to_frag_words_(es));
  header_()->event_size = es;
}

inline size_t ds50::V172xFragmentWriter::calc_event_size_words_(size_t nAdcs) {
  return adcs_to_words_(nAdcs) + header_size_words();
}

inline size_t ds50::V172xFragmentWriter::adcs_to_words_(size_t nAdcs) {
  auto mod(nAdcs % adcs_per_word_());
  return (mod == 0) ?
    nAdcs / adcs_per_word_() :
    nAdcs / adcs_per_word_() + 1;
}

inline size_t ds50::V172xFragmentWriter::words_to_frag_words_(size_t nWords) {
  size_t mod = nWords % words_per_frag_word_();
  return mod ?
    nWords / words_per_frag_word_() + 1 :
    nWords / words_per_frag_word_();
}

inline ds50::V172xFragment::Header * ds50::V172xFragmentWriter::header_() {
  assert(frag_.dataSize() >= words_to_frag_words_(header_size_words()));
  return reinterpret_cast<ds50::V172xFragment::Header *>(&*frag_.dataBegin());
}

#endif /* ds50daq_DAQ_DS50V172xFragmentWriter_hh */
