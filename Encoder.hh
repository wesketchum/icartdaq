#ifndef ds50daq_Compression_Encoder_hh
#define ds50daq_Compression_Encoder_hh

#include <istream>

#include "ds50daq/Compression/Properties.hh"
#include "ds50daq/Compression/SymTable.hh"

namespace ds50 {
  class Encoder;
}

class ds50::Encoder {
public:
  explicit Encoder(SymTable const &);

  // returns the number of bits in the out buffer
  reg_type operator()(ADCCountVec const & in, DataVec & out);
  reg_type operator()(adc_type const * beg, adc_type const * last, DataVec & out);

private:
  SymTable syms_;
};

#endif /* ds50daq_Compression_Encoder_hh */
