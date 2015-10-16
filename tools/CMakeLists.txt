cet_script(setupDemoEnvironment.sh DemoControl.rb startSystem.sh
manageSystem.sh start2x2System.sh
manage2x2System.sh start2x2x2System.sh manage2x2x2System.sh
manage1x2x2System.sh start1x2x2System.sh
CreateNewFragmentGenerator.rb  generateEventBuilderMain.rb
generateWFViewer.rb generateEventBuilder.rb demo_utilities.rb 
generateToy.rb generateUDP.rb
generatePattern.rb generateNormal.rb generateAscii.rb generateAggregator.rb 
generateAggregatorMain.rb generateV1720.rb generateCompression.rb generateBoardReaderMain.rb RunDriver.rb)


art_make_exec(demo_driver
  LIBRARIES
  artdaq_Application
  artdaq_DAQrate
  artdaq-core_Utilities
  artdaq-core_Core
  ${ART_FRAMEWORK_ART}
  ${FHICLCPP}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  ${MPI_C_LIBRARIES}
  ${CETLIB}
  )
  

# Is this necessary?
#install_source()
