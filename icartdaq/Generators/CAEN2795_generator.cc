#include "icartdaq/Generators/CAEN2795.hh"
#include "artdaq/Application/GeneratorMacros.hh"

#include <iomanip>
#include <iterator>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

icarus::CAEN2795::CAEN2795(fhicl::ParameterSet const & ps)
  :
  CAEN2795_GeneratorBase(ps)
{
  InitializeHardware();
}


void icarus::CAEN2795::InitializeHardware(){
  physCr.reset(new PhysCrate());
  physCr->initialize();
}

BoardConf icarus::CAEN2795::GetBoardConf(){

  auto const& ps_board = ps_.get<fhicl::ParameterSet>("BoardConfig");

  BoardConf config;
  config.sampInterval = 1e-9;
  config.delayTime = 0.0;
  config.nbrSegments = 1;
  config.coupling = 3;
  config.bandwidth = 0;
  config.fullScale = ps_board.get<double>("fullScale")*0.001;
  config.thresh = ps_board.get<int>("thresh");
  config.offset = ps_board.get<int>("offset") * config.fullScale/256;
  config.offsetadc = ps_board.get<int>("offset");

  return config;
}

TrigConf icarus::CAEN2795::GetTrigConf(){

  auto const& ps_trig = ps_.get<fhicl::ParameterSet>("TriggerConfig");

  TrigConf config;
  config.trigClass = 0; // 0: Edge trigger
  config.sourcePattern = 0x00000002; // 0x00000001: channel 1, 0x00000002: channel 2
  config.trigCoupling = 0;            
  config.channel = 2;
  config.trigSlope = 0; // 0: positive, 1: negative
  config.trigLevel1 = -20.0; // In % of vertical full scale or mV if using an external trigger source.
  config.trigLevel2 = 0.0;
  config.nsamples = ps_trig.get<int>("mode")*1000 ;
  config.presamples = ps_trig.get<int>("trigmode")*1000 ;

  return config;
}

void icarus::CAEN2795::ConfigureStart(){

  //physCr->configureTrig(GetTrigConf());
  //physCr->configure(GetBoardConf());
  physCr->start();
}

void icarus::CAEN2795::ConfigureStop(){}

int icarus::CAEN2795::GetData(size_t & data_size, uint32_t* data_loc){

  physCr->ArmTrigger();

  std::cout << "Calling GetData..." << std::endl;

  data_size=0;

  physCr->waitData();
  while(physCr->dataAvail()){
    auto data_ptr = physCr->getData();
    std::cout << "Got the data! It has size " << ntohl(data_ptr->Header.packSize) << std::endl;
    std::cout << "Current data size in fragment is " << data_size << std::endl;
    std::cout << "Data ptr is " << data_ptr << std::endl;
    std::cout << "End Data ptr is " << data_ptr+ntohl(data_ptr->Header.packSize) << std::endl;
    std::cout << "Data loc is " << data_loc << std::endl;
    std::cout << "Data loc now is " << data_loc+data_size << std::endl;

    std::copy((char*)data_ptr,
	      (char*)data_ptr+ntohl(data_ptr->Header.packSize),
	      (char*)data_loc+data_size);
    data_size += ntohl(data_ptr->Header.packSize);
  }

  return 0;
}

statpack icars::CAEN2795::GetStatPack()
{
  statpack pack;
  memcpy(&(pack.token), "STAT", 4);
  //pack->crateid = htonl(158);
  pack.crateid = htonl(physCr->getId());
  pack.memstat1 = 0;
  pack.memstat2 = 0;
  pack.size = htonl(28);

  std::cout << "statpack initilized..." << std::endl;

  return pack;
}

DEFINE_ARTDAQ_COMMANDABLE_GENERATOR(icarus::CAEN2795) 
