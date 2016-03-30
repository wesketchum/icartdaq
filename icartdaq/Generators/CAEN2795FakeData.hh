#ifndef icartdaq_Generators_CAEN2795FAKEDATA_hh
#define icartdaq_Generators_CAEN2795FAKEDATA_hh

#include "icartdaq/Generators/CAEN2795_GeneratorBase.hh"

#include <random>

namespace icarus {    

  class CAEN2795FakeData : public icarus::CAEN2795_GeneratorBase {
  public:

    explicit CAEN2795FakeData(fhicl::ParameterSet const & ps);

  private:

    void ConfigureStart(); //called in start()
    void ConfigureStop();  //called in stop()

    int GetData(size_t&,uint32_t*);       //called in getNext_()

    std::mt19937 engine_;
    std::unique_ptr<std::uniform_int_distribution<int>> uniform_distn_;


  };
}

#endif /* artdaq_demo_Generators_CAEN2795_FAKEDATA_hh */
