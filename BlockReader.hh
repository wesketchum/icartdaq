#ifndef ds50daq_Compression_BlockReader_hh
#define ds50daq_Compression_BlockReader_hh

#include "ds50daq/Compression/Properties.hh"
#include <iosfwd>

namespace ds50 {
  class BlockReader;
}

class ds50::BlockReader {
public:
  explicit BlockReader(std::istream &);

  // number of words read and placed into out is returned
  reg_type next(ADCCountVec & out);

private:
  std::istream * ist_;
  ADCCountVec buffer_;
};

#endif /* ds50daq_Compression_BlockReader_hh */
