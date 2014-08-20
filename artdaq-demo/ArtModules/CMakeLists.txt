art_make(NO_PLUGINS # WAnt different link lists for modues
 LIB_LIBRARIES artdaq-core-demo_Overlays artdaq-demo_Products
)

#art_make(NO_PLUGINS # WAnt different link lists for modues
#  LIB_LIBRARIES artdaq-core-demo_Overlays 
#)

 foreach (mod Baseline Converter Summer)
   simple_plugin(${mod} "module" artdaq-demo_ArtModules)
 endforeach()

 simple_plugin(Compression "module"
   artdaq-demo_Compression
   artdaq-demo_Products
   ${ARTDAQ_UTILITIES}
   ${ARTDAQ_DAQDATA} 
   ${ARTDAQ-CORE_DATA}
   ${MF_MESSAGELOGGER} 
   ${MF_UTILITIES}
   )

 simple_plugin(Decompression "module"
   artdaq-demo_Compression
   artdaq-demo_Products
   ${ARTDAQ_UTILITIES}
   ${ARTDAQ_DAQDATA} 
   ${ARTDAQ-CORE_DATA}
   ${MF_MESSAGELOGGER} 
   ${MF_UTILITIES}
   )

 simple_plugin(DataCharacterizer "module")
 simple_plugin(EventDump "module")
simple_plugin(ToyDump "module")
simple_plugin(CompressionChecker "module" ${ARTDAQ_DAQDATA} ${ARTDAQ-CORE_DATA})

# Subdirectories.
add_subdirectory(DQM)
