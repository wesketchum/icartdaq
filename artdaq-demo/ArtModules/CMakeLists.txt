include_directories(${MPI_INCLUDE_PATH})

simple_plugin(Compression "module" artdaq-demo_Compression artdaq-demo_DAQ 
        ${ARTDAQ_DAQDATA} 
        ${MF_MESSAGELOGGER} 
        ${MF_UTILITIES} )

simple_plugin(Decompression "module" artdaq-demo_Compression artdaq-demo_DAQ 
        ${ARTDAQ_DAQDATA} 
        ${MF_MESSAGELOGGER} 
        ${MF_UTILITIES} )

simple_plugin(DataCharacterizer "module" artdaq-demo_DAQ)
simple_plugin(EventDump "module" artdaq-demo_DAQ)
