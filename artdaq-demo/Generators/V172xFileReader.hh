#ifndef artdaq_demo_Generators_V172xFileReader_hh
#define artdaq_demo_Generators_V172xFileReader_hh

#include "artdaq-demo/Overlays/FragmentType.hh"
#include "artdaq-demo/Overlays/V172xFragment.hh"
#include "artdaq/DAQdata/FragmentGenerator.hh"
#include "artdaq/DAQdata/Fragments.hh"
#include "fhiclcpp/fwd.h"

#include <atomic>
#include <string>
#include <utility>
#include <vector>

namespace demo {
  // V172xFileReader reads DS50 events from a file or set of files.

  class V172xFileReader : public artdaq::FragmentGenerator {
  public:
    explicit V172xFileReader(fhicl::ParameterSet const &);

  private:
    bool getNext_(artdaq::FragmentPtrs & output) override;
    std::vector<artdaq::Fragment::fragment_id_t> fragmentIDs_() override;
    bool requiresStateMachine_() const override {return true;}
    void start_() override {should_stop_.store(false);}
    void pause_() override {}
    void resume_() override {}
    void stop_() override {should_stop_.store(true);}

    void produceSecondaries(artdaq::FragmentPtrs & frags) const;
    artdaq::FragmentPtr
    convertFragment(artdaq::Fragment const & source,
                    demo::FragmentType dType) const;

    // Configuration.
    std::vector<std::string> const fileNames_;
    uint64_t const max_set_size_bytes_;
    int const max_events_;
    FragmentType primary_type_;
    std::vector<FragmentType> secondary_types_;
    bool const size_in_words_; // To cope with malformed files.
    std::vector<artdaq::Fragment::fragment_id_t> const fragment_ids_;
    V172xFragment::adc_type const seed_;

    // State
    size_t events_read_;
    std::pair<std::vector<std::string>::const_iterator, uint64_t> next_point_;
    std::atomic<bool> should_stop_;
    std::independent_bits_engine<std::minstd_rand, 2, V172xFragment::adc_type> twoBits_;

  protected:
    bool should_stop() {return should_stop_.load();}
  };
}

#endif /* artdaq_demo_Generators_V172xFileReader_hh */
