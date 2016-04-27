#include "A2795Board.h"
#include "structures.h"
#include "veto.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define WRITE 1
#define READ 0

#define HOST 1

A2795Board::A2795Board(int nbr, int id) : 
  boardNbr(nbr), serialNbr(0),busNbr(0),slotNbr(0),preSamples(0),mThre(0),bdhandle(0),boardId(id),nSamples(0)
{






#ifndef _simulate_
// Acqrs_getInstrumentData(boardId, boardname, &serialNbr, &busNbr, &slotNbr);
  int ret;
  ret=CAENComm_OpenDevice(CAENComm_OpticalLink, 0, nbr, 0, &bdhandle);
      if (ret != CAENComm_Success) boardId=-1;
      else 
      {
         int status;
         //CAENComm_Write32(bdhandle, A_Signals, SIGNALS_SWRESET);
         CAENComm_Read32(bdhandle, A_StatusReg,(uint32_t*) &status);
         printf("board %d status %d\n",boardId,status);
         boardId=status&0x7;
         
        //CAENComm_Write32(bdhandle, A_ControlReg_Clear, CTRL_TTLINK_MODE);
//    CAENComm_Write32(bdhandle, A_ControlReg_Set, CTRL_ACQRUN);
      }
#else
#endif
 serialNbr=boardNbr;
 printf ("BoardId %x serial %d bus %d slot %d\n",boardId,serialNbr, busNbr, slotNbr);
}

A2795Board::~A2795Board()
{
  if (bdhandle!=-1)
  {
    CAENComm_Write32(bdhandle, A_Signals, SIGNALS_TTLINK_EOR);
    CAENComm_CloseDevice(bdhandle);
  }
}

#ifndef _simulate_

// Configures the board.
// Input (param): BoardConf conf - structure with the configuration parameters.
void
A2795Board::configure (BoardConf conf)
{		

  //int status;


        printf( "Configuring Board \n");
 

       CAENComm_Write32(bdhandle, A_RELE, RELE_TP_DIS);


       //status = Acqrs_calibrate(boardId);
       //AcqUtils::checkStatus(status, "Calibrated.");




    // Configure timebase


 printf ( "Conf Board %d offs %f thre %f \n",boardNbr,conf.coff[boardNbr*2],
    conf.cthre[boardNbr*2]);
  printf ( "              offs %f thre %f \n",conf.coff[(boardNbr*2)+1],
    conf.cthre[(boardNbr*2)+1]);
 
        printf("off %f thre %f \n", conf.coff[(boardNbr)*2],conf.cthre[(boardNbr)*2]);
        printf("off %f thre %f\n", conf.coff[(boardNbr)*2+1],conf.cthre[(boardNbr)*2+1]);

       // Set DC offset
  //tobeset     CAENComm_Write32(bdhandle, A_DAC, 0x00070000 | (conf.coff[boardNbr*2] & 0xFFFF) );

	printf("A2795Board::configure(): board %d configured.\n", boardNbr);
}

// Configures the trigger parameters.
// Input (param): TrigConf conf - structure with the trigger parameters.
void
A2795Board::configureTrig (TrigConf conf)
{

      nSamples=conf.nsamples;
      preSamples = conf.presamples;





      //printf("Set nSamples to %d \n",conf.nsamples);
      //printf("Set preSamples to %d \n",conf.presamples);
	//AcqrsD1_configTrigClass(boardId, conf.trigClass, conf.sourcePattern, 0, 0, 0.0, 0.0);
	//AcqrsD1_configTrigSource(boardId, conf.channel, conf.trigCoupling, conf.trigSlope, conf.trigLevel1, conf.trigLevel2);
  
    //    ViInt32 regValue;
    //    regValue=nSamples/16;
    //    Acqrs_logicDeviceIO(boardId, "Block1Dev1", 71, 1, &regValue, WRITE, 0);

     //   regValue=24000/16; //pretrigger samples
     //   Acqrs_logicDeviceIO(boardId, "Block1Dev1", 72, 1, &regValue, WRITE, 0);
}

// Starts the DPU on the board.
void
A2795Board::startDPU ()
{

    printf("A2795Board::startDPU(): core of board (%d,%d) started.\n", boardNbr, (int)boardId);
    CAENComm_Write32(bdhandle, A_ControlReg_Set, CTRL_ACQRUN);
//CAENComm_Write32(bdhandle, A_Signals, SIGNALS_TTLINK_SOR);
}

// Tells if data are ready
// Output (return): true if there are ready data, false otherwise.
int
A2795Board::isDataRdy()
{

     int timeoutCounter = 250000; // Timeout waiting for a trigger
     int errTimeoutCounter = 500000; // Timeout for acquisition completion
     //int errTimeoutCounter = 1; // Timeout for acquisition completion
     //#ifdef _dbg_
    printf("A2795Board::isDataRdy(): BoardId %d BoardNbr %d\n",boardId, boardNbr);
    //#endif

        int status;
        bool done=false;


	while (!done && errTimeoutCounter--)
	{
	//	AcqrsD1_acqDone(boardId, &done); // Poll for the end of the acquisition
        //      Acqrs_logicDeviceIO(boardId, "Block1Dev1", 69, 1, &regValue, READ, 0);
             CAENComm_Read32(bdhandle, A_StatusReg,(uint32_t*) &status);
             done=(status&0x10);  //has data bit 4 Status Reg
		if (!timeoutCounter--) // Trigger timeout occured
		{
			printf("A2795Board::isDataRdy(): Slow trigger...\n");
                       vetoOff();
		//AcqrsD1_forceTrig(boardId); // Force a 'manual' (or 'software') trigger
                }
	}
	return done;
}

int 
A2795Board::ArmTrigger()
{

  //#ifdef _dbg_
    printf("A2795Board::ArmTrigger(): wait a second...\n");
    //#endif


  return 0;
}

int
A2795Board::fillHeader(DataTile* buf)
{
  //int  rValue[950*1024];
  //#ifdef _dbg_
    printf("A2795Board::fillHeader(): wait a second...\n");
    //#endif

     CAENComm_Read32(bdhandle, A_ControlReg,(uint32_t*)&buf->Header.info1);
     CAENComm_Read32(bdhandle, A_StatusReg,(uint32_t*)&buf->Header.info2);
     CAENComm_Read32(bdhandle, A_NevStored,(uint32_t*)&buf->Header.info3);
     //buf->Header.info3=((int)inet_lnaof(mId))&0xff;
//   buf->Header.timeinfo= htonl((int) rValue[0]);
//   buf->Header.chID= htonl(serialNbr);

     printf("Filled header...\n");

     return 0;
}

#define BUFFER_SIZE ((4*1024)*32)+3

// Writes the data gathered by the board starting from the passed address.
// Output (param): char* buf - pointer to the memory area to write to.
int
A2795Board::getData(int channel,char* buf)
{

  (void)channel;

  //#ifdef _dbg_
    printf("A2795Board::getData(): wait a second...\n");
    //#endif

  // ### Readout the data ###
    //int status;
    int ret;
    //int xco,ii,jj,totwords;
    //int regValue,
    int rValue[950*1024];
    //char *mwaveform=(char *)buf;
  int nw;

     //  totwords=nSamples/4;

  printf("Before BLT Read call: %d, %d, %d\n",bdhandle, A_OutputBuffer,BUFFER_SIZE);

   ret = CAENComm_BLTRead(bdhandle, A_OutputBuffer,(uint32_t*) buf, BUFFER_SIZE, &nw);
   if ((ret != CAENComm_Success) && (ret != CAENComm_Terminated))
                printf("BLTReadCycle Error on Module (ret = %d)\n", ret);
 

for (int ii=0;ii<10;ii++)
     printf("%d ",rValue[ii]);
 printf("\n");
 printf("start decoding nw %d Boardid %d\n",nw,boardId);

/*  for (ii=0;ii<(nSamples/16); ii++) {
        for (jj=3;jj>=0;jj--)
        {
          unsigned char samp=((rValue[jj+(ii*4)]&0xff000000)>>24)&0xff;
          mwaveform[xco++]=samp-127;
          //if ((boardId==1)&&(channel==1)&&(xco<10)) printf("%d %d\n",xco-1,samp);
          samp=(rValue[jj+(ii*4)]&0xff0000)>>16;
          mwaveform[xco++]=samp-127;
          //if ((boardId==1)&&(channel==1)&&(xco<10)) printf("%d %d\n",xco-1,samp);
          samp=(rValue[jj+(ii*4)]&0xff00)>>8;
          mwaveform[xco++]=samp-127;
          //if ((boardId==1)&&(channel==1)&&(xco<10)) printf("%d %d\n",xco-1,samp);
          samp=(rValue[jj+(ii*4)]&0xff);
          mwaveform[xco++]=samp-127;
          //if ((boardId==1)&&(channel==1)&&(xco<10)) printf("%d %d\n",xco-1,samp);
        }

  }

*/
 return nw*4;

}


// Writes to a register on the FPGA.
// Input (param): int reg - register number
// Input (param): int n - number of values to write
// Input (param): int* buffer - pointer to the memory to read from.
// Output (return): int - outcome of the operation. See the Acqiris manual for information.
int
A2795Board::write(int reg, int value)
{
      CAENComm_Write32(bdhandle, reg, value);
//    return Acqrs_logicDeviceIO(boardId, "Block1Dev1", reg, n, buffer, 1, 0);
 return 0;
}

// Reads from a register on the FPGA.
// Input (param): int reg - register number
// Input (param): int n - number of values to read
// Output (param): int* buffer - pointer to the memory area to write to.
// Output (return): int - outcome of the operation. See the Acqiris manual for information.
int
A2795Board::read(int reg, int *value)
{
   return CAENComm_Read32(bdhandle, reg,(uint32_t *) value);
    // return Acqrs_logicDeviceIO(boardId, "Block1Dev1", reg, n, buffer, 0, 0);
}
#endif

// For testing purposes. If _simulate_ is defined, it should work without the hardware.
#ifdef _simulate_
void A2795Board::configure (BoardConf conf) {

  printf ( "Conf Board %d offs %f thre %f \n",boardNbr,conf.coff[boardNbr*2],
    conf.cthre[boardNbr*2]);
  printf ( "              offs %f thre %f \n",conf.coff[(boardNbr*2)+1],
    conf.cthre[(boardNbr*2)+1]);

}

void
A2795Board::configureTrig (TrigConf conf)
{

      nSamples=conf.nsamples;
      preSamples = conf.presamples;

      printf("Set nSamples to %d \n",conf.nsamples);
      printf("Set preSamples to %d \n",conf.presamples);
}

void A2795Board::startDPU () {}

int 
A2795Board::isDataRdy()
{
  //#ifdef _dbg_
    printf("A2795Board::isDataRdy(): wait a second...\n");
    sleep(1);
    //#endif
    return true;
}

int
A2795Board::ArmTrigger()
{
  return 0;
}
int
A2795Board::fillHeader(DataTile* buf)
{
   //buf->Header.timeinfo= htonl((int) rValue[0]);
   buf->Header.chID= htonl(serialNbr);
}

int
A2795Board::getData(int boa,char* buf)
{
  //#ifdef _dbg_
    printf("A2795Board::getData(): filling with %d (%c) for %d bytes\n", id, (char)(id+'0'), nSamples);
    //#endif
    for (int i = 0; i < nSamples; i++)
        buf[i] = (char)(boardNbr+'0');
  return nSamples;
}

#endif
