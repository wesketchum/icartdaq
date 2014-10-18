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
  artdaq-core-demo_Overlays
  cetlib
  )

art_make_exec(BuildHuffmanTable LIBRARIES artdaq-demo_Compression)
art_make_exec(DecodeHuffman LIBRARIES artdaq-demo_Compression)
art_make_exec(EncodeHuffman LIBRARIES artdaq-demo_Compression)
art_make_exec(CodesMain LIBRARIES artdaq-demo_Compression)
art_make_exec(MakeSample LIBRARIES artdaq-demo_Compression)
art_make_exec(tester NO_INSTALL LIBRARIES artdaq-demo_Compression)

install_headers()
install_source()
