#ifndef icartdaq_Generators_PhysCrateData_hh
#define icartdaq_Generators_PhysCrateData_hh

#include "ica_base/ica_base/PhysCrate.hh"
#include "icartdaq/Generators/PhysCrateData_GeneratorBase.hh"
//#include <memory>

namespace icarus {    

  class PhysCrateData : public icarus::PhysCrateData_GeneratorBase {
    
  public:
    
    explicit PhysCrateData(fhicl::ParameterSet const & ps);
    
  private:
    
    void ConfigureStart(); //called in start()
    void ConfigureStop();  //called in stop()

    int  GetData(size_t&,uint32_t*);       //called in getNext_()
    void FillStatPack(statpack&);

    void InitializeHardware();
    BoardConf GetBoardConf();
    TrigConf GetTrigConf();

    std::unique_ptr<PhysCrate> physCr;
  };
}


#endif /* artdaq_demo_Generators_PhysCrateData_hh */
