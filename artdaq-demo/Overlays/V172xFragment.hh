#ifndef artdaq_demo_Overlays_V172xFragment_hh
#define artdaq_demo_Overlays_V172xFragment_hh

#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq/DAQdata/features.hh"

#include <ostream>
#include <vector>

// Fragment overlay class for DS50 data.
namespace demo {
  class V172xFragment;

  std::ostream & operator << (std::ostream &, V172xFragment const &);
}

class demo::V172xFragment {
  public:
    struct metadata {
      typedef uint32_t data_t;

      uint32_t serial_number;
      char roc_firmware[6], amc_firmware[6];
      uint8_t v17xx_model, sample_bits;
      uint16_t sample_rate_MHz;
      uint32_t record_length;
      uint16_t post_trigger;
      uint16_t unused[3];

      static size_t const size_words = 8ul;
    };
#if USE_MODERN_FEATURES
    static_assert (sizeof (metadata) == metadata::size_words * sizeof (metadata::data_t), "metadata size changed");
#endif /* USE_MODERN_FEATURES */

    struct Header {
      typedef uint32_t data_t;

      typedef uint32_t event_size_t;
      typedef uint8_t channel_mask_t;
      typedef uint16_t pattern_t;
      typedef uint8_t  board_id_t;
      typedef uint32_t event_counter_t;
      typedef uint32_t trigger_time_tag_t;

      uint32_t event_size : 28;
      uint32_t unused_1   :  4;

      uint32_t channel_mask :  8;
      uint32_t pattern      : 16;
      uint32_t unused_2     :  3;
      uint32_t board_id     :  5;

      uint32_t event_counter : 24;
      uint32_t reserved      :  8;

      uint32_t trigger_time_tag : 32;

      static size_t const size_words = 4ul;
    };
    typedef uint16_t adc_type;
    typedef std::vector<adc_type> ADCCountVec;

    explicit V172xFragment(artdaq::Fragment const & f);

    size_t event_size() const;
    Header::channel_mask_t channel_mask() const;
    bool channel_present(int ch) const; // Throws on failure.
    unsigned int enabled_channels() const;
    Header::pattern_t pattern() const;
    Header::board_id_t board_id() const;
    Header::event_counter_t event_counter() const;
    Header::trigger_time_tag_t trigger_time_tag() const;

    size_t total_adc_values() const;
    size_t adc_values_for_channel() const;
    adc_type const * dataBegin() const;
    adc_type const * dataEnd() const;
    adc_type const * chDataBegin(int ch) const; // Throws on failure.
    adc_type const * chDataEnd(int ch) const;  // Throws on failure.

    bool fastVerify(int daq_adc_bits) const;
    adc_type const * findBadADC(int daq_adc_bits) const;
    void checkADCData(int daq_adc_bits) const; // Throws on failure.

#if USE_MODERN_FEATURES
    static constexpr size_t header_size_words();
    static constexpr size_t adc_range(int daq_adc_bits);
#endif /* USE_MODERN_FEATURES */

protected:
#if USE_MODERN_FEATURES
  static constexpr size_t adcs_per_word_();
#endif /* USE_MODERN_FEATURES */

  Header const * header_() const;

private:
  artdaq::Fragment const & data_;
};

inline demo::V172xFragment::V172xFragment(artdaq::Fragment const & f): data_(f) {}

inline size_t demo::V172xFragment::event_size() const { return header_()->event_size; }

inline demo::V172xFragment::Header::channel_mask_t demo::V172xFragment::channel_mask() const { return header_()->channel_mask; }

inline demo::V172xFragment::Header::pattern_t demo::V172xFragment::pattern() const { return header_()->pattern; }

inline demo::V172xFragment::Header::board_id_t demo::V172xFragment::board_id() const { 
return header_()->board_id; }

inline demo::V172xFragment::Header::event_counter_t demo::V172xFragment::event_counter() const { return header_()->event_counter; }

inline demo::V172xFragment::Header::trigger_time_tag_t demo::V172xFragment::trigger_time_tag() const { return header_()->trigger_time_tag; }

#if USE_MODERN_FEATURES
inline size_t demo::V172xFragment::total_adc_values() const {
  return (event_size() - header_size_words()) * adcs_per_word_();
}
#endif /* USE_MODERN_FEATURES */

inline size_t demo::V172xFragment::adc_values_for_channel() const { return total_adc_values() / enabled_channels(); }

inline demo::V172xFragment::adc_type const * demo::V172xFragment::dataBegin() const {
  return reinterpret_cast<adc_type const *>(header_() + 1);
}

inline demo::V172xFragment::adc_type const * demo::V172xFragment::dataEnd() const {
  return dataBegin() + total_adc_values();
}

inline demo::V172xFragment::adc_type const * demo::V172xFragment::chDataEnd(int ch) const {
    return chDataBegin (ch) + adc_values_for_channel ();
}

#if USE_MODERN_FEATURES

inline bool demo::V172xFragment::fastVerify(int daq_adc_bits) const {
  return (findBadADC(daq_adc_bits) == dataEnd());
}

inline demo::V172xFragment::adc_type const * demo::V172xFragment::findBadADC(int daq_adc_bits) const {
  return std::find_if(dataBegin(), dataEnd(), [&](adc_type const adc) -> bool { return (adc >> daq_adc_bits); });
}

inline constexpr size_t demo::V172xFragment::header_size_words() { return V172xFragment::Header::size_words; }

inline constexpr size_t demo::V172xFragment::adc_range(int daq_adc_bits) { return (1ul << daq_adc_bits); }

inline constexpr size_t demo::V172xFragment:: adcs_per_word_() {
  return sizeof(V172xFragment::Header::data_t) / sizeof(adc_type);
}

inline demo::V172xFragment::Header const * demo::V172xFragment::header_() const {
  return reinterpret_cast<V172xFragment::Header const *>(data_.dataBeginBytes());
}

#endif /* USE_MODERN_FEATURES */

#endif /* artdaq_demo_Overlays_V172xFragment_hh */
