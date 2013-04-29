#ifndef artdaq_demo_Overlays_V1190FragmentWriter_hh
#define artdaq_demo_Overlays_V1190FragmentWriter_hh
#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq-demo/Overlays/V1190Fragment.hh"

namespace demo {
  class V1190FragmentWriter: public V1190Fragment {
    public:
      V1190FragmentWriter(artdaq::Fragment &, float resolution_ns = 0.8, Word::geo_t geo = 0.8);

      void add_header (Word::event_count_t event_count);
      void add_hit (Word::channel_t ch, float leading_ns, float trailing_ns);
      void add_trailer ();

    private:
      artdaq::Fragment & frag_;
      float resolution_ns_;
      Word::geo_t geo_;
      size_t word_count_;

      word_t build_global_header (Word::event_count_t event_count);
      word_t build_measurement (Word::channel_t ch, float time_ns, bool leading);
      word_t build_trailer ();
      word_t & word(size_t i);
  };

  inline V1190FragmentWriter::V1190FragmentWriter(artdaq::Fragment &frag, float resolution_ns, Word::geo_t geo): V1190Fragment(frag), frag_(frag), resolution_ns_(resolution_ns), geo_(geo), word_count_(0) { }

  inline void V1190FragmentWriter::add_header (Word::event_count_t event_count) {
    word (word_count_++) = build_global_header (event_count);
  }

  inline void V1190FragmentWriter::add_hit (Word::channel_t ch, float leading_ns, float trailing_ns) {
    word (word_count_++) = build_measurement (ch, leading_ns, true);
    word (word_count_++) = build_measurement (ch, trailing_ns, false);
  }

  inline void V1190FragmentWriter::add_trailer () { word (word_count_++) = build_trailer (); }

  inline V1190Fragment::word_t V1190FragmentWriter::build_global_header (Word::event_count_t event_count) {
    return (word_t(Word::global_header) << 27) | (word_t(event_count & 0x3fffff) << 5) | (geo_ & 0x1f);
  }

  inline V1190Fragment::word_t V1190FragmentWriter::build_measurement (Word::channel_t ch, float time_ns, bool leading) {
    return (word_t(Word::measurement_w) << 27) | (leading ? 0 : (1 << 26)) | (word_t(ch & 0x7f) << 19) | (int(time_ns / resolution_ns_) & 0x7ffff);
  }

  inline V1190Fragment::word_t V1190FragmentWriter::build_trailer () {
    return (word_t(Word::trailer) << 27) | (word_t(word_count_ & 0xffff) << 5) | (geo_ & 0x1f);
  }

  inline V1190Fragment::word_t & V1190FragmentWriter::word(size_t i) {
    size_t wc = ceil (i * sizeof (word_t) / sizeof (artdaq::RawDataType));
    if (frag_.size () < wc) frag_.resize (wc);
    return (reinterpret_cast<V1190Fragment::word_t*>(&*frag_.dataBegin()))[i];
  }
}
#endif /* artdaq_demo_Overlays_V1190FragmentWriter_hh */
