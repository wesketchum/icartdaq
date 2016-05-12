#include_directories(/home/icadaq/artdaq-demo-base/artdaq-demo/artdaq-demo)
#set(HEADER_FILES /home/icadaq/artdaq-demo-base/artdaq-demo/artdaq-demo/A2795.h)

art_make_library( LIBRARY_NAME icartdaq_PhysCrate_GeneratorBase
		  SOURCE PhysCrate_GeneratorBase.cc
		  LIBRARIES
			icartdaq-core_Overlays
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

simple_plugin(PhysCrateData "generator"
  ica_base_PhysCrate
  icartdaq_PhysCrate_GeneratorBase
  icartdaq-core_Overlays
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

simple_plugin(PhysCrateFakeData "generator"
  icartdaq_PhysCrate_GeneratorBase
  icartdaq-core_Overlays
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
