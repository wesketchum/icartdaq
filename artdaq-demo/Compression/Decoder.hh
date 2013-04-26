#ifndef artdaq_demo_Compression_Decoder_hh
#define artdaq_demo_Compression_Decoder_hh

#include "artdaq-demo/Compression/Properties.hh"
#include "artdaq-demo/Compression/SymTable.hh"

namespace demo {
  class Decoder;
  constexpr auto neg_one = ~(0ul);
}

class demo::Decoder {
public:
  Decoder(SymTable const &);

  // Overwrite vector out with the uncompressed data.
  reg_type operator()(reg_type bit_count,
                      DataVec const & in,
                      ADCCountVec & out);

  // Start writing uncompressed data at out_ptr. Caller's responsibility
  // to ensure buffer is big enough (by reading header data, for
  // example) (faster).
  reg_type operator()(reg_type bit_count,
                      DataVec::const_iterator in,
                      adc_type * out_ptr,
                      adc_type const * out_end,
		      bool use_diffs);

  void printTable(std::ostream & ost) const;

private:

  void buildTable();

  size_t addNode() {
    table_.push_back(neg_one);
    table_.push_back(neg_one);
    size_t rc = last_;
    last_ += 2;
    return rc;
  }

  SymTable syms_;
  DataVec table_;
  size_t head_;
  size_t last_;
};

#endif /* artdaq_demo_Compression_Decoder_hh */
