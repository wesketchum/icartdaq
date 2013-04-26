#ifndef ds50daq_Compression_Decoder_hh
#define ds50daq_Compression_Decoder_hh

#include "ds50daq/Compression/Properties.hh"
#include "ds50daq/Compression/SymTable.hh"

namespace ds50 {
  class Decoder;
  constexpr auto neg_one = ~(0ul);
}

class ds50::Decoder {
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

#endif /* ds50daq_Compression_Decoder_hh */
