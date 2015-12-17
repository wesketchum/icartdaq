simple_plugin(V172xFileReader "generator"
  artdaq-core-demo_Overlays
  artdaq_Application
  artdaq_DAQdata
  artdaq-core_Utilities
  artdaq-core_Data
  ${ART_UTILITIES}
  ${FHICLCPP}
  ${CETLIB}
  )

simple_plugin(V172xSimulator "generator"
  artdaq-core-demo_Overlays
  artdaq_Application
  artdaq_DAQdata
  artdaq-core_Utilities
  artdaq-core_Data
  ${ART_UTILITIES}
  ${FHICLCPP}
  ${CETLIB}
  )

simple_plugin(ToySimulator "generator"
  artdaq-core-demo_Overlays
  artdaq_Application
  artdaq_DAQdata
  artdaq-core_Utilities
  artdaq-core_Data
  ${ART_UTILITIES}
  ${FHICLCPP}
  ${MF_MESSAGELOGGER}
  ${MF_UTILITIES}
  ${CETLIB}
  )


#add_library(libCAENComm.so.1.2.0 STATIC /home/icadaq/CAENComm-1.2/include/CAENComm.h)
#SET_TARGET_PROPERTIES(libCAENComm.so.1.2.0 PROPERTIES LINKER_LANGUAGE C)
#LINK_DIRECTORIES(/home/icadaq/CAENComm-1.2/lib/x64/)
#target_link_libraries(libCAENComm.so.1.2.0)

#include_directories(/home/icadaq/CAENComm-1.2/include/)
#set(HEADER_FILES /home/icadaq/CAENComm-1.2/include/CAENComm.h)

include_directories(/home/icadaq/artdaq-demo-base/artdaq-demo/artdaq-demo)
set(HEADER_FILES /home/icadaq/artdaq-demo-base/artdaq-demo/artdaq-demo/A2795.h)



simple_plugin(CAEN2795 "generator"
  artdaq-core-demo_Overlays
  artdaq_Application
  artdaq_DAQdata
  artdaq-core_Utilities
  artdaq-core_Data
  CAENComm
  ${ART_UTILITIES}
  ${FHICLCPP}
  ${MF_MESSAGELOGGER}
  ${MF_UTILITIES}
  ${CETLIB}
  )

simple_plugin(NormalSimulator "generator"
  artdaq-core-demo_Overlays
  artdaq_Application
  artdaq_DAQdata
  artdaq-core_Utilities
  artdaq-core_Data
  ${ART_UTILITIES}
  ${FHICLCPP}
  ${CETLIB}
  )

simple_plugin(PatternSimulator "generator"
  artdaq-core-demo_Overlays
  artdaq_Application
  artdaq_DAQdata
  artdaq-core_Utilities
  artdaq-core_Data
  ${ART_UTILITIES}
  ${FHICLCPP} 
  ${CETLIB} 
  )

simple_plugin(AsciiSimulator "generator"
  artdaq-core-demo_Overlays
  artdaq_Application
  artdaq_DAQdata
  artdaq-core_Utilities
  artdaq-core_Data
  ${FHICLCPP}
  ${CETLIB}
  )

simple_plugin(UDPReceiver "generator"
    artdaq-core-demo_Overlays
    artdaq_Application
    artdaq_DAQdata
    artdaq-core_Utilities
    artdaq-core_Data
  ${ART_UTILITIES}
    ${FHICLCPP}
  ${MF_MESSAGELOGGER}
  ${MF_UTILITIES}
    ${CETLIB}
    )

