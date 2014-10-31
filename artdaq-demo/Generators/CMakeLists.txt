simple_plugin(V172xFileReader "generator"
  artdaq-core-demo_Overlays
  ${ARTDAQ_APPLICATION}
  ${ARTDAQ_DAQDATA}
  ${ARTDAQ-CORE_UTILITIES}
  ${ARTDAQ-CORE_DATA}
  ${ART_UTILITIES}
  ${FHICLCPP}
  ${CETLIB}
  )

simple_plugin(V172xSimulator "generator"
  artdaq-core-demo_Overlays
  ${ARTDAQ_APPLICATION}
  ${ARTDAQ_DAQDATA}
  ${ARTDAQ-CORE_UTILITIES}
  ${ARTDAQ-CORE_DATA}
  ${ART_UTILITIES}
  ${FHICLCPP}
  ${CETLIB}
  )

simple_plugin(ToySimulator "generator"
  artdaq-core-demo_Overlays
  ${ARTDAQ_APPLICATION}
  ${ARTDAQ_DAQDATA}
  ${ARTDAQ-CORE_UTILITIES}
  ${ARTDAQ-CORE_DATA}
  ${ART_UTILITIES}
  ${FHICLCPP}
  ${CETLIB}
  )

simple_plugin(NormalSimulator "generator"
  artdaq-core-demo_Overlays
  ${ARTDAQ_APPLICATION}
  ${ARTDAQ_DAQDATA}
  ${ARTDAQ-CORE_UTILITIES}
  ${ARTDAQ-CORE_DATA}
  ${ART_UTILITIES}
  ${FHICLCPP}
  ${CETLIB}
  )

simple_plugin(PatternSimulator "generator"
  artdaq-core-demo_Overlays
  ${ARTDAQ_APPLICATION}
  ${ARTDAQ_DAQDATA}
  ${ARTDAQ_UTILITIES}
  ${ART_UTILITIES}
  ${FHICLCPP} 
  ${CETLIB} 
  )
