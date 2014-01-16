#ifndef artdaq_demo_Generators_ToySimulator_hh
#define artdaq_demo_Generators_ToySimulator_hh

// ToySimulator is a type of fragment generator used to demonstrate
// the way in which users can implement their own fragment generators,
// while also providing examples of "best practices" related to the
// implementation

// ToySimulator is designed to simulate values coming in from one of
// two types of digitizer boards, one called "Toy1" and the other
// called "Toy2"; the only difference between the two boards is the #
// of bits in the ADC values they send (see ToySimulator_generator.cc
// for more)

// Some C++ conventions used:
// -Append a "_" to every private member function and variable

#include "fhiclcpp/fwd.h"
#include "artdaq/DAQdata/Fragments.hh" 
#include "artdaq/Application/CommandableFragmentGenerator.hh"
#include "artdaq-demo/Overlays/ToyFragment.hh"
#include "artdaq-demo/Overlays/FragmentType.hh"

#include <random>
#include <vector>
#include <atomic>

namespace demo {

  class ToySimulator : public artdaq::CommandableFragmentGenerator {
  public:
    explicit ToySimulator(fhicl::ParameterSet const & ps);

  private:

    // The "getNext_" function is used to implement user-specific
    // functionality; it's a mandatory override of the pure virtual
    // getNext_ function declared in CommandableFragmentGenerator

    bool getNext_(artdaq::FragmentPtrs & output) override;

    // Like "getNext_", "fragmentIDs_" is a mandatory override; it
    // returns a vector of the fragment IDs the BoardReaderMain
    // application running the object of this class is responsible for
    // in an event
    
    std::vector<artdaq::Fragment::fragment_id_t> fragmentIDs_() override;

    // FHiCL-configurable variables. Note that the C++ variable names
    // are the FHiCL variable names with a "_" appended

    std::size_t const nADCcounts_;
    FragmentType const fragment_type_;
    std::vector<artdaq::Fragment::fragment_id_t> fragment_ids_;  // Returned by fragmentIDs_()

    // State.
    std::size_t current_event_num_;
    //    std::vector<std::vector<size_t>> adc_freqs_;
    //    std::vector<std::discrete_distribution<V172xFragment::adc_type>> content_generator_;
    std::atomic<bool> should_stop_;
    std::mt19937 engine_;

  };
}

#endif /* artdaq_demo_Generators_ToySimulator_hh */
