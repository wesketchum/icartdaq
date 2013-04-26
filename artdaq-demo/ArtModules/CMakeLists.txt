simple_plugin(Compression "module" artdaq-demo_Compression artdaq-demo_Products ${ARTDAQ_UTILITIES}
        ${ARTDAQ_DAQDATA} 
        ${MF_MESSAGELOGGER} 
        ${MF_UTILITIES} )

simple_plugin(Decompression "module" artdaq-demo_Compression artdaq-demo_Products ${ARTDAQ_UTILITIES}
        ${ARTDAQ_DAQDATA} 
        ${MF_MESSAGELOGGER} 
        ${MF_UTILITIES} )

simple_plugin(DataCharacterizer "module")
simple_plugin(EventDump "module")
