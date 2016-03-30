#ifndef icartdaq_Generators_CAEN2795_hh
#define icartdaq_Generators_CAEN2795_hh

//#include "CAENComm.h"
//#include "A2795.h"
#include "packs.h"
#include "PhysCrate.h"
#include "icartdaq/Generators/CAEN2795_GeneratorBase.hh"
//#include <memory>

namespace icarus {    

  class CAEN2795 : public icarus::CAEN2795_GeneratorBase {
    
  public:
    
    explicit CAEN2795(fhicl::ParameterSet const & ps);
    
  private:
    
    void ConfigureStart(); //called in start()
    void ConfigureStop();  //called in stop()

    int GetData(size_t&,uint32_t*);       //called in getNext_()

    void InitializeHardware();
    BoardConf GetBoardConf();
    TrigConf GetTrigConf();

    std::unique_ptr<PhysCrate> physCr;
  };
}


#endif /* artdaq_demo_Generators_CAEN2795_hh */
