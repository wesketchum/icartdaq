#ifndef artdaq_demo_Overlays_ToyFragmentWriter_hh
#define artdaq_demo_Overlays_ToyFragmentWriter_hh
////////////////////////////////////////////////////////////////////////
// ToyFragmentWriter
//
// Class derived from ToyFragment which allows writes to the data (for simulation purposes)
//
////////////////////////////////////////////////////////////////////////

#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq-demo/Overlays/ToyFragment.hh"

namespace demo {
  class ToyFragmentWriter;
}

// Note the inheritance: order is important here (for construction
// initialization order).
class demo::ToyFragmentWriter: public demo::ToyFragment {
public:


  ToyFragmentWriter(artdaq::Fragment & f, 
		    std::vector<artdaq::Fragment::type_t> const & ftypes ) :
    ToyFragment(f, ftypes), artdaq_Fragment_(f) {}

  // These functions form overload sets with const functions from
  // demo::ToyFragment

  // Are these necessary? Let's see what happens when I comment them out...

  //  adc_type * dataBegin();
  //  adc_type * dataEnd();

  // We'll need to hide the const version of header in ToyFragment in
  // order to be able to perform writes

  Header * header_() {
    // This assert could be improved
    // assert(frag_.dataSize() >= words_to_frag_words_(hdr_size_words()));
    return reinterpret_cast<Header *>(&*artdaq_Fragment_.dataBegin());
  }

//  void setChannelMask(demo::ToyFragment::Header::channel_mask_t mask);
//  void setPattern(demo::ToyFragment::Header::pattern_t pattern);

  void set_hdr_run_number(Header::run_number_t run_number) { 
    header_()->run_number = run_number;
  }

  // Note that this also sets the artdaq::Fragment's sequence_id value

  void set_hdr_event_number(Header::event_number_t event_number) { 
    header_()->event_number = event_number;
    artdaq_Fragment_.setSequenceID( event_number );
  }



  //  void setBoardID(demo::ToyFragment::Header::board_id_t id);
  //  void setEventCounter(demo::ToyFragment::Header::event_counter_t event_counter);
  //  void setTriggerTimeTag(demo::ToyFragment::Header::trigger_time_tag_t tag);

  void resize(size_t nAdcs);

private:
  size_t calc_event_size_words_(size_t nAdcs);

  static size_t adcs_to_words_(size_t nAdcs);
  static size_t words_to_frag_words_(size_t nWords);

  //  demo::ToyFragment::Header * header_();
  //  Header * header_();

  // Note that this non-const reference hides the const reference in the base class
  artdaq::Fragment & artdaq_Fragment_;
};

// {
  //}
  


//inline demo::ToyFragmentWriter::ToyFragmentWriter(artdaq::Fragment & frag): ToyFragment(frag), frag_(frag) { }

// Can we get away with using the parent class's const dataBegin() and dataEnd() ?

// inline demo::ToyFragment::adc_type * demo::ToyFragmentWriter::dataBegin() {
//   assert(frag_.dataSize() > words_to_frag_words_(hdr_size_words()));
//   return reinterpret_cast<adc_type *>(header_() + 1);
// }

// inline demo::ToyFragment::adc_type * demo::ToyFragmentWriter::dataEnd() {
//   return dataBegin() + total_adc_values();
// }


inline void demo::ToyFragmentWriter::resize(size_t nAdcs) {
  auto es(calc_event_size_words_(nAdcs));
  artdaq_Fragment_.resize(words_to_frag_words_(es));
  header_()->event_size = es;
}

inline size_t demo::ToyFragmentWriter::calc_event_size_words_(size_t nAdcs) {
  return adcs_to_words_(nAdcs) + hdr_size_words();
}

inline size_t demo::ToyFragmentWriter::adcs_to_words_(size_t nAdcs) {
  auto mod(nAdcs % adcs_per_word_());
  return (mod == 0) ?
    nAdcs / adcs_per_word_() :
    nAdcs / adcs_per_word_() + 1;
}

inline size_t demo::ToyFragmentWriter::words_to_frag_words_(size_t nWords) {
  size_t mod = nWords % words_per_frag_word_();
  return mod ?
    nWords / words_per_frag_word_() + 1 :
    nWords / words_per_frag_word_();
}

// inline demo::ToyFragment::Header * demo::ToyFragmentWriter::header_() {
//   assert(frag_.dataSize() >= words_to_frag_words_(hdr_size_words()));
//   return reinterpret_cast<demo::ToyFragment::Header *>(&*frag_.dataBegin());
// }

#endif /* artdaq_demo_Overlays_ToyFragmentWriter_hh */
