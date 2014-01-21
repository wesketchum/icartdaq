#ifndef artdaq_demo_Overlays_ToyFragmentWriter_hh
#define artdaq_demo_Overlays_ToyFragmentWriter_hh

////////////////////////////////////////////////////////////////////////
// ToyFragmentWriter
//
// Class derived from ToyFragment which allows writes to the data (for
// simulation purposes). Note that for this reason it contains
// non-const members which hide the const members in its parent class,
// ToyFragment, including its reference to the artdaq::Fragment
// object, artdaq_Fragment_, as well as its functions pointing to the
// beginning and end of ADC values in the fragment, dataBegin() and
// dataEnd()
//
////////////////////////////////////////////////////////////////////////

#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq-demo/Overlays/ToyFragment.hh"

#include <iostream>

namespace demo {
  class ToyFragmentWriter;
}


class demo::ToyFragmentWriter: public demo::ToyFragment {
public:


  ToyFragmentWriter(artdaq::Fragment & f) :
    ToyFragment(f), artdaq_Fragment_(f) {}

  // These functions form overload sets with const functions from
  // demo::ToyFragment

  adc_t * dataBegin();
  adc_t * dataEnd();

  // We'll need to hide the const version of header in ToyFragment in
  // order to be able to perform writes

  Header * header_() {
    return reinterpret_cast<Header *>(&*artdaq_Fragment_.dataBegin());
  }

  void set_hdr_run_number(Header::run_number_t run_number) { 
    header_()->run_number = run_number;
  }

  void resize(size_t nAdcs);

private:
  size_t calc_event_size_words_(size_t nAdcs);

  static size_t adcs_to_words_(size_t nAdcs);
  static size_t words_to_frag_words_(size_t nWords);

  // Note that this non-const reference hides the const reference in the base class
  artdaq::Fragment & artdaq_Fragment_;
};


inline demo::ToyFragment::adc_t * demo::ToyFragmentWriter::dataBegin() {
  assert(frag_.dataSize() > words_to_frag_words_(header_size_words()));
  return reinterpret_cast<adc_t *>(header_() + 1);
}

inline demo::ToyFragment::adc_t * demo::ToyFragmentWriter::dataEnd() {
  return dataBegin() + total_adc_values();
}


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

#endif /* artdaq_demo_Overlays_ToyFragmentWriter_hh */
