#ifndef artdaq_demo_Overlays_V1190Fragment_hh
#define artdaq_demo_Overlays_V1190Fragment_hh

#include "artdaq/DAQdata/Fragment.hh"
#include "cetlib/exception.h"

#include <ostream>
#include <vector>

namespace demo {
  class V1190Fragment {
    public:
      typedef uint32_t word_t;

      struct Word {
	enum types {
	  global_header = 0x8,
	  tdc_header = 1,
	  measurement_w = 0,
	  tdc_trailer = 3,
	  tdc_error = 4,
	  extended_time_tag_w = 0x11,
	  trailer = 0x10,
	  filler = 0x18
	};

	typedef uint8_t type_t;
	typedef uint32_t event_count_t;
	typedef uint8_t geo_t;
	typedef uint8_t tdc_t;
	typedef uint16_t event_id_t;
	typedef uint16_t bunch_id_t;
	typedef uint8_t channel_t;
	typedef uint32_t measurement_t;
	typedef uint16_t error_flags_t;
	typedef uint32_t extended_time_tag_t;
	typedef uint8_t status_t;

	type_t type () const { return (word_ >> 27) & 0x1f; }
	event_count_t event_count ()	const { return check_type (global_header), ((word_ >> 5) & 0x3fffff); }
	geo_t geo ()			const { return check_type (global_header, tdc_trailer, trailer), (word_ & 0x1f); }
	tdc_t tdc ()			const { return check_type (tdc_header, tdc_trailer, tdc_error), ((word_ >> 24) & 3); }
	event_id_t event_id ()		const { return check_type (tdc_header, tdc_trailer),    (word_ & 0xfff); }
	bunch_id_t bunch_id ()		const { return check_type (tdc_header), ((word_ >> 12) & 0xfff); }
	bool trailing_edge ()		const { return check_type (measurement_w), (word_ & 1<<26); }
	channel_t channel ()		const { return check_type (measurement_w), ((word_ >> 19 ) & 0x7f); }
	measurement_t measurement ()	const { return check_type (measurement_w), (word_ & 0x7ffff); }
	size_t tdc_word_count ()	const { return check_type (tdc_trailer), (word_ & 0xfff); }
	error_flags_t error_flags ()	const { return check_type (tdc_error), ((word_ >> 12) & 0xfff); }
	extended_time_tag_t extended_time_tag () const { return check_type (extended_time_tag_w), (word_ & 0x7ffffff); }
	status_t status	()		const {	return check_type (trailer), ((word_ >> 24) & 3); }
	size_t word_count ()		const { return check_type (trailer), ((word_ >> 5) & 0xffff); }
	template <typename... T> void check_type (T... args) const;

	word_t word_;
      };
      static_assert (sizeof (Word) == sizeof (word_t), "word is not alligned to 32bit");

      V1190Fragment (artdaq::Fragment const & f): data_(f) {}

      Word const * dataBegin () const;
      Word const * dataEnd () const;
      //Word const * chDataBegin (int ch = -1) const; // Throws on failure.
      //Word const * chDataEnd (int ch = -1) const;  // Throws on failure.

    private:
      artdaq::Fragment const & data_;
  };


  std::ostream & operator << (std::ostream &, V1190Fragment const &);
  std::ostream & operator << (std::ostream &, V1190Fragment::Word const &);
}

template <typename... T> void demo::V1190Fragment::Word::check_type (T... args) const {
  std::vector<types> v = {args...};
  bool found = false;
  for (auto x: v) if (type () == x) found = true;
  if (!found) throw cet::exception("WrongV1190Word") << "type is " << int(type ()) << " while request is for " << v[0];
}

inline demo::V1190Fragment::Word const * demo::V1190Fragment::dataBegin () const {
  return reinterpret_cast<V1190Fragment::Word const *>(&*data_.dataBegin());
}
inline demo::V1190Fragment::Word const * demo::V1190Fragment::dataEnd () const {
  return dataBegin () + data_.dataSize () * sizeof (artdaq::RawDataType) / sizeof (Word);
}


#endif /* artdaq_demo_Overlays_V1190Fragment_hh */
