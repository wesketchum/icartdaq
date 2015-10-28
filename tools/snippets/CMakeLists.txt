cet_script(makeConfigs.rb)

file(GLOB ERBFCL_TEMPLATES "*.erb.fcl")
install( FILES ${ERBFCL_TEMPLATES}
         DESTINATION ${flavorqual_dir}/../snippets )

file(GLOB FCL_TEMPLATES "templates/*.fcl")
install( FILES ${FCL_TEMPLATES}
         DESTINATION ${flavorqual_dir}/../snippets/templates )

art_make_exec(readfhicl
  LIBRARIES
  ${FHICLCPP}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  ${CETLIB}
  )

# Is this necessary?
#install_source()
