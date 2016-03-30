#ifndef _PhysCrate_
#define _PhysCrate_

#include <stdio.h>
#include <netinet/in.h>


#include "structures.h"
#include "A2795Board.h" 

class PhysCrate {
    private:
        A2795Board** boards;
        
        static const int headSize = 28;
        
        int nBoards;
        char * tilebuf; 
         int presBoard;
         int mip;
    public:
        PhysCrate();
        ~PhysCrate();
        
        void initialize();
        void configure (BoardConf conf);  
        void configureTrig(TrigConf conf);
        void waitData();
	void start();
        void ArmTrigger();
        bool dataAvail();
        DataTile* getData(); 
        int getId();

};

#endif
