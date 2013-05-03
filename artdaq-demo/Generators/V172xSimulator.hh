#ifndef artdaq_demo_Generators_V172xSimulator_hh
#define artdaq_demo_Generators_V172xSimulator_hh

#include "fhiclcpp/fwd.h"
#include "artdaq/DAQdata/Fragments.hh"
#include "artdaq/DAQdata/FragmentGenerator.hh"
#include "artdaq-demo/Overlays/V172xFragment.hh"
#include "artdaq-demo/Overlays/FragmentType.hh"

#include <random>
#include <vector>
#include <atomic>

namespace demo {
  // V172xSimulator creates simulated DS50 events, with data
  // distributed according to a "histogram" provided in the configuration
  // data.

  class V172xSimulator : public artdaq::FragmentGenerator {
  public:
    explicit V172xSimulator(fhicl::ParameterSet const & ps);

  private:
    bool getNext_(artdaq::FragmentPtrs & output) override;
    std::vector<artdaq::Fragment::fragment_id_t> fragmentIDs_() override;
    bool requiresStateMachine_() const override {return true;}
    void start_() override {should_stop_.store(false); current_event_num_=0;}
    void pause_() override{}
    void resume_() override {}
    void stop_() override {should_stop_.store(true);}

    // Configuration.
    std::size_t const nChannels_;
    FragmentType const fragment_type_;
    std::vector<artdaq::Fragment::fragment_id_t> fragment_ids_;

    // State.
    std::size_t current_event_num_;
    std::mt19937 engine_;
    std::vector<std::vector<size_t>> adc_freqs_;
    std::vector<std::discrete_distribution<V172xFragment::adc_type>> content_generator_;
    std::atomic<bool> should_stop_;

  protected:
    bool should_stop() {return should_stop_.load();}
  };
}

#endif /* artdaq_demo_Generators_V172xSimulator_hh */
