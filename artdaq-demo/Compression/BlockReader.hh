#ifndef artdaq_demo_Compression_BlockReader_hh
#define artdaq_demo_Compression_BlockReader_hh

#include "artdaq-demo/Compression/Properties.hh"
#include <iosfwd>

namespace demo {
  class BlockReader;
}

class demo::BlockReader {
public:
  explicit BlockReader(std::istream &);

  // number of words read and placed into out is returned
  reg_type next(ADCCountVec & out);

private:
  std::istream * ist_;
  ADCCountVec buffer_;
};

#endif /* artdaq_demo_Compression_BlockReader_hh */
