#include "PhysCrate.h"
#include "veto.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

#define kMaxSize 950000
#define kMaxBoards 8
#define EVEN 0x4556454E
#define DATA 0x44415441
#define STAT 0x53544154


PhysCrate::PhysCrate() {}

PhysCrate::~PhysCrate() 
{ 
  //	delete(tilebuf);

	delete[] tilebuf;
}

#ifndef _simulate_
// Initializes the crate detecting the boards
void
PhysCrate::initialize() 
{
    printf("PhysCrate::initialize(): Initializing crate.\n");
    //int status;
    //int boardId;
    
    boards = new A2795Board*[kMaxBoards];
    
    nBoards=0;
    do {
        boards[nBoards]=new A2795Board(nBoards,nBoards);
        if (boards[nBoards]->boardId>-1)
        { 
              printf("PhysCrate::initialize(): Created board (%d, %d)\n", nBoards, boards[nBoards]->boardId);
             nBoards++;
        }
        else break;
    } while (nBoards<kMaxBoards);

    presBoard=0;
    mip=0;

    printf("PhysCrate::initialize(): %d A2795 boards found.\n", nBoards);
    
    
    char mhost[128];
    gethostname(mhost,128);

    struct hostent *hp = gethostbyname(mhost);
    printf("got local name %s\n",mhost);

    if (hp == NULL) {
             (void) printf("host %s not found\n", mhost);
             //exit (3);
      mip=158;
     }
    else {
       char ** p = hp->h_addr_list;
       in_addr in;
       memcpy(&in.s_addr, *p, sizeof (in.s_addr));
        printf("nietntoa %s\n",inet_ntoa(in));
       sscanf(inet_ntoa(in),"%d",&mip);
    }
    printf("Set localID\n");
    int size = sizeof(DataTile::Header)+2*kMaxSize*nBoards;
    tilebuf = new char[size];
    //int size = 2*kMaxSize*nBoards;
    //tilebuf = new char[size];
    //DataTile tile;
    //tile.data = new char[size];

}
#endif

int
PhysCrate::getId()
{
 return mip;
}

#ifdef _simulate_
void
PhysCrate::initialize()
{
    nBoards = 8;
    boards = new A2795Board*[nBoards];
    for (int i = 0; i < nBoards; i++)
    {
        boards[i] = new A2795Board(i,0);
        printf("PhysCrate::initialize(): Created board %d\n");
    }
  int size = sizeof(DataTile::Header)+2*kMaxSize*nBoards;
  tilebuf = new char[size];
}
#endif

// Configures the boards
void
PhysCrate::configure(BoardConf conf)
{
    for (int i = 0; i < nBoards; i++)
    {
        boards[i]->configure(conf);
    }
    printf("PhysCrate::configure(): Board parameters set.\n");
} // configure()

// Configures the trigger parameters
void 
PhysCrate::configureTrig(TrigConf conf)
{
    for (int i = 0; i < nBoards; i++)
    {
        boards[i]->configureTrig(conf);
    }
    printf("PhysCrate::configureTrig(): Trigger parameters set.\n");
}

void
PhysCrate::start()
{
  /*
    for (int i = 0; i < nBoards; i++)
    {
        boards[i]->startDPU();
        boards[i]->write(A_Signals, SIGNALS_TTLINK_ALIGN);
    }
  */
  //boards[0]->write(A_Signals, SIGNALS_TTLINK_ALIGN);
   boards[0]->write(A_Signals, SIGNALS_TTLINK_COMMA);
   sleep(3);
   boards[0]->write(A_Signals, SIGNALS_TTLINK_SOR);

   int status;
   boards[0]->read(A_ControlReg,&status);
   printf("ControlReg: %d\n",status);
   boards[0]->read(A_ControlReg2,&status);
   printf("ControlReg2: %d\n",status);
   boards[0]->read(A_DebugReg,&status);
   printf("DebugReg: %d\n",status);
   boards[0]->read(A_StatusReg,&status);
   printf("StatusReg: %d\n",status);


  //vetoOn();
//  for (int i = 0; i< nBoards; i++)
//    boards[i]->ArmTrigger();
  //vetoOff();
  
}


// Waits until the boards give a signal of Data Ready
void
PhysCrate::waitData()
{
  //#ifdef _dbg_
    printf("PhysCrate::waitData() on %d boards\n", nBoards);
    //#endif

#ifdef AUTO_TRIGGER
  //boards[0]->write(A_Signals, SIGNALS_TTLINK_GTRG);
#endif

    // for (int i = 0; i < nBoards; i++)
    for (int i = 0; i < 1; i++)
    // for (int i = nBoards-1; i < nBoards; i++)
    {
      //#ifdef _dbg_
        printf("PhysCrate::waitData(): waiting for board %d\n", i);
	//#endif
        printf("PhysCrate::waitData(): waiting for board %d\n", i);

        while(!(boards[i]->isDataRdy()));
        
	//#ifdef _dbg_
        printf("Board %d ready.\n", i);
	//#endif
    }
} // waitData()


bool
PhysCrate::dataAvail()
{
 if (presBoard<nBoards) return true;
  return false;
}
// Returns a pointer to a dynamically allocated memory area with the data of the event.
DataTile*
PhysCrate::getData()
{
   int status;
   boards[0]->read(A_ControlReg,&status);
   printf("ControlReg: %d\n",status);
   boards[0]->read(A_ControlReg2,&status);
   printf("ControlReg2: %d\n",status);
   boards[0]->read(A_DebugReg,&status);
   printf("DebugReg: %d\n",status);
   boards[0]->read(A_StatusReg,&status);
   printf("StatusReg: %d\n",status);
   //const int size = 2*kMaxSize*nBoards;
  //char* tilebuf = new char[size];

  DataTile* tile = reinterpret_cast<DataTile*>(tilebuf);
  //DataTile tile;
  //tile.data = new char[size];

  // read all boards
   int i=presBoard;

  //for (int i = 0; i < nBoards; i++)
  //{
    //boards[i]->getData(1,tile->data + 2*i*nSamples);
    //boards[i]->getData(2,tile->data + (2*i+1)*nSamples);

//printf ("nSamp %d \n",boards[i]->nSamples);

   tile->Header.token = htonl(DATA);
   tile->Header.info1 = htonl(0xcdfcdf);

   printf("Header.info1 %d\n",tile->Header.info1);

   int nSamples=boards[i]->getData(1,tile->data);
printf ("nSamp %d \n",nSamples);
//    boards[i]->getData(2,tile->data + boards[i]->nSamples);

  //}

    //tile->Header.packSize = htonl(size);
    //tile->Header.packSize = htonl(2*nSamples+sizeof(DataTile));
    tile->Header.packSize = htonl(nSamples+sizeof(DataTile::Header));
    //tile->Header.packSize = nSamples+sizeof(DataTile::Header);
    //tile->Header.packSize = htonl(2*boards[i]->nSamples+sizeof(DataTile));
    //tile->Header.chID = htonl(mip+i*256); 
    boards[i]->fillHeader(tile);
    presBoard++ ;
    return tile;
}


void
PhysCrate::ArmTrigger()
{
  vetoOn();
  for (int i = 0; i< nBoards; i++)
    boards[i]->ArmTrigger();
  //vetoOff();

  presBoard=0;

} 

#if FALSE
// Returns a pointer to a dynamically allocated memory area with the data of the event.
DataTile*
PhysCrate::getData()
{
  int size = sizeof(DataTile::Header)+2*nSamples*nBoards;
  char* buf = new char[size];
  DataTile* tile = (DataTile*) buf;
  // read all boards
  for (int i = 0; i < nBoards; i++)
  {
    boards[i]->getData(1,tile->data + 2*i*(sizeof(DataTile::Header)+nSamples));
    tile->Header.packSize = htonl(sizeof(DataTile::Header)+nSamples);

    boards[i]->getData(2,tile->data + (2*i+1)*(sizeof(DataTile::Header)+nSamples));
    tile->Header.packSize = htonl(sizeof(DataTile::Header)+nSamples);
  
  }
  //for (int i = 0; i< nBoards; i++)
  //  boards[i]->startDPU();

  vetoOn();
  for (int i = 0; i< nBoards; i++)
    boards[i]->ArmTrigger();
  //vetoOff();
  
  return tile;    
} // getData()
#endif
