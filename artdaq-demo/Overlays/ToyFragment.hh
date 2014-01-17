#ifndef artdaq_demo_Overlays_ToyFragment_hh
#define artdaq_demo_Overlays_ToyFragment_hh

#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq/DAQdata/features.hh"
#include "cetlib/exception.h"

#include <ostream>
#include <vector>

// Implementation of "ToyFragment", an artdaq::Fragment overlay class used for pedagogical purposes

namespace demo {
  class ToyFragment;

  std::ostream & operator << (std::ostream &, ToyFragment const &);
}

class demo::ToyFragment {
  public:

  // The ToyFragment represents its data through the adc_t type, which
  // is a typedef of an unsigned 16-bit integer. Note that since there
  // are two types of ToyFragment ("TOY1" and "TOY2", declared in
  // FragmentType.hh), the ADC type needs to be large enough to hold
  // the ADC count with the highest number of bits.

  typedef uint16_t adc_t;

  // The "Metadata" struct is used to store info primarily related to
  // the upstream hardware environment from where the fragment came

  // "data_t" is a typedef of the fundamental unit of data the
  // metadata structure thinks of itself as consisting of; it can give
  // its size via the static "size_words" variable ( ToyFragment::Metadata::size_words )

  struct Metadata {

    typedef uint32_t data_t;

    uint32_t board_serial_number : 16;
    uint32_t num_adc_bits : 8;
    uint32_t unused : 8; // 16 + 8 + 8 == 32 bits
    
    static size_t const size_words = 1ul;
  };

  // #if USE_MODERN_FEATURES
  static_assert (sizeof (Metadata) == Metadata::size_words * sizeof (Metadata::data_t), "ToyFragment::Metadata size changed");
  // #endif /* USE_MODERN_FEATURES */

  
  // The "Header" struct contains "metadata" specific to the fragment
  // which is not hardware-related

  // Header::data_t describes the standard size of a data type not
  // just for the header data, but the physics data beyond it; the
  // size of the header in units of Header::data_t is given by
  // "size_words", and the size of the fragment beyond the header in
  // units of Header::data_t is given by "event_size"

  // Notice only the first 28 bits of the first 32-bit unsigned
  // integer in the Header is used to hold the event_size ; this means
  // that you can't represent a fragment larger than 2**28 units of
  // data_t, or 1,073,741,824 bytes

  struct Header {
    typedef uint32_t data_t;

    typedef uint32_t event_size_t;  
    typedef uint32_t run_number_t;
    typedef uint32_t event_number_t;

    uint32_t event_size : 28;
    uint32_t unused_1   :  4;

    uint32_t run_number : 32;
    uint32_t event_number : 32;

    static size_t const size_words = 3ul;
  };

  static_assert (sizeof (Header) == Header::size_words * sizeof (Header::data_t), "ToyFragment::Header size changed");

  // The constructor simply sets its const private member "artdaq_Fragment_"
  // to refer to the artdaq::Fragment object

  ToyFragment(artdaq::Fragment const & f ) : artdaq_Fragment_(f) {}

  // const getter functions for the data in the header

  Header::event_size_t hdr_event_size() const { return header_()->event_size; } 
  Header::run_number_t hdr_run_number() const { return header_()->run_number; }
  Header::event_number_t hdr_event_number() const { return header_()->event_number; }
  static constexpr size_t hdr_size_words() { return Header::size_words; }

  size_t total_adc_values() const; // The number of ADC values describing data beyond the header

  adc_t const * dataBegin() const; // Start of the ADC values
  adc_t const * dataEnd() const; // End of the ADC values

  // Functions to check if any ADC values are corrupt.

  bool fastVerify(int daq_adc_bits) const;
  adc_t const * findBadADC(int daq_adc_bits) const;
  void checkADCData(int daq_adc_bits) const; // Throws if ADC appears corrupt

  //#if USE_MODERN_FEATURES
  //    static constexpr size_t header_size_words();
  //  static constexpr size_t adc_range(int daq_adc_bits);

  size_t adc_range(int daq_adc_bits) {
    return (1ul << daq_adc_bits );
  }


  //#endif /* USE_MODERN_FEATURES */

  protected:
  //#if USE_MODERN_FEATURES

  static constexpr size_t adcs_per_word_() {
    return sizeof(Header::data_t) / sizeof(adc_t);
  }

  static constexpr size_t words_per_frag_word_() {
    return sizeof(artdaq::Fragment::value_type) / sizeof(Header::data_t);
  }

  //#endif /* USE_MODERN_FEATURES */

  Header const * header_() const;

private:
  artdaq::Fragment const & artdaq_Fragment_;
};

inline demo::ToyFragment::Header const * demo::ToyFragment::header_() const {
  return reinterpret_cast<ToyFragment::Header const *>(&*artdaq_Fragment_.dataBegin());
}


//#if USE_MODERN_FEATURES
inline size_t demo::ToyFragment::total_adc_values() const {
  return (hdr_event_size() - hdr_size_words()) * adcs_per_word_();
}
//#endif /* USE_MODERN_FEATURES */

//inline size_t demo::ToyFragment::adc_values_for_channel() const { return total_adc_values() / enabled_channels(); }

inline demo::ToyFragment::adc_t const * demo::ToyFragment::dataBegin() const {
  return reinterpret_cast<adc_t const *>(header_() + 1);
}

inline demo::ToyFragment::adc_t const * demo::ToyFragment::dataEnd() const {
  return dataBegin() + total_adc_values();
}

//#if USE_MODERN_FEATURES



inline demo::ToyFragment::adc_t const * demo::ToyFragment::findBadADC(int daq_adc_bits) const {
  return std::find_if(dataBegin(), dataEnd(), [&](adc_t const adc) -> bool { return (adc >> daq_adc_bits); });
  //  return std::find_if(dataBegin(), dataEnd(), 
  //		      [&](adc_t const adc) -> 
  //		      bool { return (adc >> artdaq_Fragment_.metadata()->num_adc_bits ); } );
}

inline bool demo::ToyFragment::fastVerify(int daq_adc_bits) const {
  return (findBadADC(daq_adc_bits) == dataEnd());
}


// Utility functions -- should I put these in the body of the class?

// inline constexpr size_t demo::ToyFragment::adc_range() { 
//   return (1ul << artdaq_Fragment_.metadata()->num_adc_bits ); }


//#endif /* USE_MODERN_FEATURES */

#endif /* artdaq_demo_Overlays_ToyFragment_hh */
