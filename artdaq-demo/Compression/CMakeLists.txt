# ======================================================================
#
# Build/install script
#
# ======================================================================

# ----------------------------------------------------------------------
# Build this project's library:

#set( LIB_NAME ${PROJECT_NAME} )
set( LIB_NAME ds50daq_Compression )

string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC)
if(BTYPE_UC STREQUAL DEBUG)
  add_definitions(-DDEBUGME)
endif()

set( LIB_SOURCES 
  Accum.cc
  BlockReader.cc
  Codes.cc
  Decoder.cc
  Encoder.cc
  ExpDecode.cc
  HuffmanTable.cc
  SymProb.cc
  SymTable.cc
   )

art_make_library(SOURCE ${LIB_SOURCES}
  LIBRARIES
  ds50daq_DAQ
  ${ARTDAQ_DAQRATE}
  ${ARTDAQ_DAQDATA}
	${CETLIB}
  )

# ----------------------------------------------------------------------
# Build this project's executables:
link_libraries( ${LIB_NAME} ) # All execs need this.

art_make_exec(NAME BuildHuffmanTable LIBRARIES ${LIB_NAME})
art_make_exec(NAME DecodeHuffman LIBRARIES ${LIB_NAME})
art_make_exec(NAME EncodeHuffman LIBRARIES ${LIB_NAME})
# art_make_exec(EncodeDS50)
# art_make_exec(NAME BuildRiceTable LIBRARIES ${LIB_NAME})
art_make_exec(NAME CodesMain LIBRARIES ${LIB_NAME})
art_make_exec(NAME MakeSample LIBRARIES ${LIB_NAME})
art_make_exec(NAME tester NO_INSTALL LIBRARIES ${LIB_NAME})

# ----------------------------------------------------------------------
# Install this project's headers, sources, library, and executables:

install_headers()
install_source()
install( TARGETS ${LIB_NAME}
         DESTINATION ${flavorqual_dir}/lib )
install( TARGETS MakeSample
         DESTINATION ${flavorqual_dir}/bin )

#
# ======================================================================
