#ifndef artdaq_demo_Compression_Encoder_hh
#define artdaq_demo_Compression_Encoder_hh

#include <istream>

#include "artdaq-demo/Compression/Properties.hh"
#include "artdaq-demo/Compression/SymTable.hh"

namespace demo {
  class Encoder;
}

class demo::Encoder {
public:
  explicit Encoder(SymTable const &, bool use_diffs=false);

  // returns the number of bits in the out buffer
  reg_type operator()(ADCCountVec const & in, DataVec & out);
  reg_type operator()(adc_type const * beg, adc_type const * last, DataVec & out);

private:
  SymTable syms_;
  bool use_diffs_;
};

#endif /* artdaq_demo_Compression_Encoder_hh */
