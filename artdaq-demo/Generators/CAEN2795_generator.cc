#include "artdaq-demo/Generators/CAEN2795.hh"

#include "art/Utilities/Exception.h"
#include "artdaq/Application/GeneratorMacros.hh"
#include "cetlib/exception.h"
#include "artdaq-core-demo/Overlays/ToyFragment.hh"
#include "artdaq-core-demo/Overlays/ToyFragmentWriter.hh"
#include "artdaq-core-demo/Overlays/FragmentType.hh"
#include "fhiclcpp/ParameterSet.h"
#include "artdaq-core/Utilities/SimpleLookupPolicy.h"

#include "CAENComm.h"
//#include "keyb.h"
//#include "keyb.c"

#include <fstream>
#include <iomanip>
#include <iterator>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define PBLT_SIZE     (61440)                     // in byte
#define RECORD_LENGTH (4*1024)                    // waveform length (in samples)
#define EVENT_SIZE    ((RECORD_LENGTH * 32) + 3)  // Event Size in lwords
#define NEV_READ      (1)                         // num of event to read with one BLT
#define BUFFER_SIZE   (EVENT_SIZE * NEV_READ)     // readout buffer size

namespace {

  size_t typeToADC(demo::FragmentType type)
  {
    switch (type) {
    case demo::FragmentType::TOY1:
      return 12;
      break;
    case demo::FragmentType::TOY2:
      return 14;
      break;
    default:
      throw art::Exception(art::errors::Configuration)
        << "Unknown board type "
        << type
        << " ("
        << demo::fragmentTypeToString(type)
        << ").\n";
    };
  }

}



demo::CAEN2795::CAEN2795(fhicl::ParameterSet const & ps)
  :
  CommandableFragmentGenerator(ps),
  nADCcounts_(ps.get<size_t>("nADCcounts", 600000)),
  fragment_type_(toFragmentType(ps.get<std::string>("fragment_type"))),
  throttle_usecs_(ps.get<size_t>("throttle_usecs", 100000)),
  throttle_usecs_check_(ps.get<size_t>("throttle_usecs_check", 10000)),
  engine_(ps.get<int64_t>("random_seed", 314159)),
  uniform_distn_(new std::uniform_int_distribution<int>(0, pow(2, typeToADC( fragment_type_ ) ) - 1 ))
{

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
  int board_num  = 1;
  //uint32_t data;
  CAENComm_ErrorCode ret = CAENComm_Success;
  int i;

  std::cout << "In initialization. We have a positive ret code: " << ret << std::endl;

  // sveglio il MASTER: conet node deve essere 0
  ret = CAENComm_OpenDevice(CAENComm_OpticalLink, 0, 0, 0, &LinkHandle[0]);

  std::cout << "After OpenDevice. We have a ret code: " << ret << std::endl;
  
  if (ret != CAENComm_Success) 
    {
      printf("Error opening the Optical Node (%d). \n", 0);
      return;
    }
  else
    {
      std::cout << "After OpenDevice. Ret was success!  " << ret << std::endl;
      LinkInit[0] = 1;
    }
  // sveglio eventuali SLAVE
  if (board_num > 1)
    {
      for (i = 1; i < board_num; i++)
        {
	  ret = CAENComm_OpenDevice(CAENComm_OpticalLink, 0, i, 0, &LinkHandle[i]);
	  if (ret != CAENComm_Success) 
	    {
	      printf("Error opening the Optical Node (%d). \n", board_num);
	      return;
	    }
	  else {LinkInit[i] = 1;}
	}
    }
  CAENComm_Write32(LinkHandle[0], A_Signals, SIGNALS_TTLINK_ALIGN);
  std::cout<<"LinkInit[0] = "<<LinkInit[0]<<std::endl;
  usleep(100);
//-------------------------------------------------------------------------------------  
//------------------------------------------------------------------------------------- 

  // Check and make sure that the fragment type will be one of the "toy" types
  
  std::vector<artdaq::Fragment::type_t> const ftypes = 
    {FragmentType::TOY1, FragmentType::TOY2 };

  if (std::find( ftypes.begin(), ftypes.end(), fragment_type_) == ftypes.end() ) {
    throw cet::exception("Error in CAEN2795: unexpected fragment type supplied to constructor");
  }

  if (throttle_usecs_ > 0 && (throttle_usecs_check_ >= throttle_usecs_ ||
			      throttle_usecs_ % throttle_usecs_check_ != 0) ) {
    throw cet::exception("Error in CAEN2795: disallowed combination of throttle_usecs and throttle_usecs_check (see CAEN2795.hh for rules)");
  }
    

  start();
}

//taken from SW_MULTI::test7
void demo::CAEN2795::start() {

  int loc_conet_node = 0;
  //int loc_conet_handle;
  int ret;
  unsigned int data;
  //unsigned int data_ap;
  
  printf("BOARD CONET NODE: (Default is %d) ", loc_conet_node);
  //scanf("%d", &loc_conet_node);
  
  if (loc_conet_node <8){
    // la scheda non è inizializzata
    if ((!LinkInit[loc_conet_node])) {  
      // inizializzo
      ret = CAENComm_OpenDevice(CAENComm_OpticalLink, 0, loc_conet_node, 0, &LinkHandle[loc_conet_node]);  // init locale
      if (ret != CAENComm_Success)
	printf("Error opening the Conet Node (ret = %d). A2795 not found\n", ret);
      else{
	LinkInit[loc_conet_node] = 1;
      }
    }
    
    // RUN/NOT RUN
    ret = CAENComm_Read32(LinkHandle[loc_conet_node], A_ControlReg, &data);
    if ((data & CTRL_ACQRUN) == 0){ 
      ret = CAENComm_Write32(LinkHandle[loc_conet_node], A_ControlReg_Set, CTRL_ACQRUN);
      printf("\nBOARD RUNNING...\n");
    }
    else{
      ret = CAENComm_Write32(LinkHandle[loc_conet_node], A_ControlReg_Clear, CTRL_ACQRUN);
      printf("\nBOARD NOT RUNNING...\n");
    }		
  }
  else{  // Parlo col MASTER
    // il master non è inizializzato
    if ((!LinkInit[0])) {  
      // inizializzo
      ret = CAENComm_OpenDevice(CAENComm_OpticalLink, 0, 0, 0, &LinkHandle[0]);  // init locale
      if (ret != CAENComm_Success)
	printf("Error opening the Conet Node 0 (ret = %d). A2795 MASTER not found\n", ret);
      else{
	LinkInit[0] = 1;
      }
    }
    
    usleep(100);
    
    // leggo se il master è già in run per decidere se dare SOR o EOR
    CAENComm_Read32(LinkHandle[0], A_StatusReg, &data);
    
    //data_ap = (data & STATUS_RUNNING);
    
    if ((data & STATUS_RUNNING) == 0){
      // Start Acq
      CAENComm_Write32(LinkHandle[0], A_Signals, SIGNALS_TTLINK_SOR);  
      printf("\nTTLink SOR...\n");
    }
    else{
      CAENComm_Write32(LinkHandle[0], A_Signals, SIGNALS_TTLINK_EOR);  
      printf("\nTTLink EOR...\n");
    }
    CAENComm_Read32(LinkHandle[0], A_StatusReg, &data);
    //data_ap = (data & STATUS_RUNNING);
    
  }
  
  usleep(100);
}

void demo::CAEN2795::stop() {
  start();
}

// ---------------------------------------------------------------------------
// Read a block of data (32 events???) and get the waveforms
// ---------------------------------------------------------------------------
int ReadEvent(int handle,int *nb)
{
  uint32_t* buff;   // Max ev size = 4K sample per channel = 4*1024*32+3
  int ret, nw, nword, s;
  
  //int32_t OneSample[32];
  //unsigned short CHeSample;
  //unsigned short CHoSample;
  int record_length=4096;
  
  
  
  // malloc BLT buffer 
  buff = (uint32_t*)malloc(BUFFER_SIZE*4);
  
  
  // Execute Readout
  nword=0;
  
  ret = CAENComm_BLTRead(handle, A_OutputBuffer, buff, BUFFER_SIZE, &nw);
  if ((ret != CAENComm_Success) && (ret != CAENComm_Terminated)){
    printf("BLTReadCycle Error on Module (ret = %d)\n", ret);
    //getch();
    //return 2;
  }
  nword=nword+(nw);
  auto Evnum = buff[0] & 0x00FFFFFF;
  auto Timestamp = buff[1];

  std::cout << "EventNumber and Timestamp are " << Evnum << ", " << Timestamp << std::endl;
  
  for(s=0; s<record_length; s++){  // HACK: iterare su pi� eventi
    // 1 sample 32 word
    for(nw=0; nw<32; nw++){
      //OneSample[nw] = buff[(32*s)+nw+2];
    }
    /*
    for(ch=0;ch<64;ch++){
      if ((ch%2)==0){ // ch even
	CHeSample = (unsigned short)(0x0000FFFF & OneSample[(ch/2)]);
	acq.wave[ch][s] = CHeSample;
      }
      else {          // ch odd
	CHoSample = (unsigned short)((0xFFFF0000 & OneSample[(ch/2)]) >> 16);
	acq.wave[ch][s] = CHoSample;
      }
    }
    */
  }
  *nb=nword*4;
  
  free(buff);
  
  return 0;
}

bool demo::CAEN2795::getNext_(artdaq::FragmentPtrs & frags) {

  
  unsigned int data;
  CAENComm_Read32(LinkHandle[0], A_StatusReg, &data);
  data = (data & STATUS_DRDY);

  int nb;
  if ((data & STATUS_DRDY) != 0)
    ReadEvent(LinkHandle[0],&nb);

  std::cout << "nb val is " << nb << std::endl;


  // JCF, 9/23/14

  // If throttle_usecs_ is greater than zero (i.e., user requests a
  // sleep interval before generating the pseudodata) then during that
  // interval perform a periodic check to see whether a stop request
  // has been received

  // Values for throttle_usecs_ and throttle_usecs_check_ will have
  // been tested for validity in constructor

  if (throttle_usecs_ > 0) {
    size_t nchecks = throttle_usecs_ / throttle_usecs_check_;

    for (size_t i_c = 0; i_c < nchecks; ++i_c) {
      usleep( throttle_usecs_check_ );

      if (should_stop()) {
	return false;
      }
    }
  } else {
    if (should_stop()) {
      return false;
    }
  }

  // Set fragment's metadata

  ToyFragment::Metadata metadata;
  metadata.board_serial_number = 999;
  metadata.num_adc_bits = typeToADC(fragment_type_);

  // And use it, along with the artdaq::Fragment header information
  // (fragment id, sequence id, and user type) to create a fragment

  // We'll use the static factory function 

  // artdaq::Fragment::FragmentBytes(std::size_t payload_size_in_bytes, sequence_id_t sequence_id,
  //  fragment_id_t fragment_id, type_t type, const T & metadata)

  // which will then return a unique_ptr to an artdaq::Fragment
  // object. The advantage of this approach over using the
  // artdaq::Fragment constructor is that, if we were to want to
  // initialize the artdaq::Fragment with a nonzero-size payload (data
  // after the artdaq::Fragment header and metadata), we could provide
  // the size of the payload in bytes, rather than in units of the
  // artdaq::Fragment's RawDataType (8 bytes, as of 3/26/14). The
  // artdaq::Fragment constructor itself was not altered so as to
  // maintain backward compatibility.

  std::size_t initial_payload_size = 0;

  frags.emplace_back( artdaq::Fragment::FragmentBytes(initial_payload_size,  
						      ev_counter(), fragment_id(),
						      fragment_type_, metadata) );

  // Then any overlay-specific quantities next; will need the
  // ToyFragmentWriter class's setter-functions for this

  ToyFragmentWriter newfrag(*frags.back());

  newfrag.set_hdr_run_number(999);

  newfrag.resize(nADCcounts_);

  // And generate nADCcounts ADC values ranging from 0 to max with an
  // equal probability over the full range (a specific and perhaps
  // not-too-physical example of how one could generate simulated
  // data)

  std::generate_n(newfrag.dataBegin(), nADCcounts_,
  		  [&]() {
  		    return static_cast<ToyFragment::adc_t>
  		      ((*uniform_distn_)( engine_ ));
  		  }
  		  );

  if(metricMan_ != nullptr) {
    metricMan_->sendMetric("Fragments Sent",ev_counter(), "Events", 3);
  }
  // Check and make sure that no ADC values in this fragment are
  // larger than the max allowed

  newfrag.fastVerify( metadata.num_adc_bits );

  ev_counter_inc();

  return true;
}

// The following macro is defined in artdaq's GeneratorMacros.hh header
DEFINE_ARTDAQ_COMMANDABLE_GENERATOR(demo::CAEN2795) 
