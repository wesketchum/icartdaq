#include_directories(/home/icadaq/artdaq-demo-base/artdaq-demo/artdaq-demo)
#set(HEADER_FILES /home/icadaq/artdaq-demo-base/artdaq-demo/artdaq-demo/A2795.h)

art_make_library( LIBRARY_NAME icartdaq_CAEN2795_A2795Board
		  SOURCE Cudpux.cc A2795Board.cc veto.cc
		  LIBRARIES
		        CAENComm
		)

art_make_library( LIBRARY_NAME icartdaq_CAEN2795_PhysCrate
		  SOURCE PhysCrate.cc
		  LIBRARIES
		        icartdaq_CAEN2795_A2795Board
		)

art_make_library( LIBRARY_NAME icartdaq_CAEN2795_GeneratorBase
		  SOURCE CAEN2795_GeneratorBase.cc
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

simple_plugin(CAEN2795 "generator"
  icartdaq_CAEN2795_PhysCrate
  icartdaq_CAEN2795_GeneratorBase
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

simple_plugin(CAEN2795FakeData "generator"
  icartdaq_CAEN2795_GeneratorBase
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
