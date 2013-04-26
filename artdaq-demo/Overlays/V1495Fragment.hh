#ifndef artdaq_demo_Overlays_V1495Fragment_hh
#define artdaq_demo_Overlays_V1495Fragment_hh

#include "artdaq/DAQdata/Fragment.hh"
#include <ostream>
#include <sys/time.h>

// Fragment overlay class for DS50 data.
namespace demo {
  class V1495Fragment {
    public:
//#pragma pack(push,2)
      struct metadata {
	timeval trigger_time;
	uint16_t serial;
	uint8_t firmware_version;
	uint8_t unused[5];
      };
//#pragma pack(pop) 
      static_assert (sizeof (metadata) == 24, "metadata size changed");

      enum trigger_types {
	laser = 2,
	external = 3,
	pulser = 4,
	random = 5,
	tpc = 7,
	veto_ls = 8,
	veto_cw = 9,
      };
      typedef uint16_t run_number_t;
      typedef uint16_t trigger_id_t;
      typedef uint32_t register_t;
      typedef uint64_t trigger_pattern_t;
      typedef uint32_t count_t;

      explicit V1495Fragment(artdaq::Fragment const & f);

      static size_t pod_size () { return sizeof (POD); }

      size_t event_size () const { return pod_ ()->length * sizeof (uint32_t); }

      run_number_t run_number () const { return pod_ ()->run_number; }

      trigger_types trigger_type () const { return trigger_types((pod_ ()->trigger_id >> 12) & 0xf); }
      count_t trigger_counter () const { return pod_ ()->trigger_counter; }

      register_t status_register () const { return pod_ ()->status_register; }
      register_t trigger_mask () const { return pod_ ()->trigger_mask; }
      register_t majority_setup () const { return pod_ ()->majority_setup; }

      count_t gps_coarse () const { return pod_ ()->gps_coarse; }
      count_t gps_fine () const { return pod_ ()->gps_fine; }
      count_t gps_pps () const { return pod_ ()->gps_pps; }

      trigger_pattern_t trigger_pattern_raw () const { return (trigger_pattern_t(pod_ ()->tpc_trigger_pattern_high) << 32) | pod_ ()->tpc_trigger_pattern_low; }
      trigger_pattern_t trigger_pattern () const { return (trigger_pattern_t(pod_ ()->tpc_trigger_pattern_high & 0xff) << 30) | ((pod_ ()->tpc_trigger_pattern_low > 1) & (0x7fff << 15)) | (pod_ ()->tpc_trigger_pattern_low & 0x7fff); }
      bool channel_in_trigger (int ch) const;

      count_t total_inhibit_time_us () const { return pod_ ()->total_inhibit_time_us; }
      count_t incremental_inhibit_time_20ns () const { return pod_ ()->incremental_inhibit_time_20ns; }
      count_t live_time_20ns () const { return pod_ ()->live_time_20ns; }

      bool fast_verify () const;

    protected:
      struct POD {
	uint32_t length : 8, fw_type : 8, run_number : 16;
	uint32_t trigger_id : 16, status_register : 16;
	uint32_t trigger_mask : 16, majority_setup : 16;
	uint32_t gps_coarse : 24, zero1 : 8;
	uint32_t gps_fine;
	uint32_t gps_pps;
	uint32_t tpc_trigger_pattern_low;
	uint32_t tpc_trigger_pattern_high : 8, zero4 : 24;
	uint32_t trigger_counter;
	uint32_t unused : 8, zero6 : 24;
	uint32_t total_inhibit_time_us;
	uint32_t incremental_inhibit_time_20ns;
	uint32_t live_time_20ns;
	uint32_t caen_bug;
      };

      POD const * pod_ () const;
    private:
      artdaq::Fragment const & data_;
  };

  std::ostream & operator << (std::ostream &, V1495Fragment const &);
}
inline demo::V1495Fragment::V1495Fragment(artdaq::Fragment const & f): data_(f) {}

inline demo::V1495Fragment::POD const * demo::V1495Fragment::pod_() const {
  return reinterpret_cast<V1495Fragment::POD const *>(&*data_.dataBegin());
}

inline bool demo::V1495Fragment::fast_verify () const { 
  return pod_size () == event_size () && ((pod_ ()->trigger_id & 0xfff) == (trigger_counter () & 0xfff));
}

#endif /* artdaq_demo_Overlays_V1495Fragment_hh */
