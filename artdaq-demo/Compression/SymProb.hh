#ifndef ds50daq_Compression_SymProb_hh
#define ds50daq_Compression_SymProb_hh

/*
  jbk - old note from code for incr():
  "this is not the correct sym probability calculation"
*/

#include <ostream>
#include "ds50daq/Compression/Properties.hh"

namespace ds50 {
  struct SymProb;

  std::ostream & operator<<(std::ostream & ost, SymProb const & s);

  typedef std::vector<SymProb> SymsVec;

  void calculateProbs(ADCCountVec const & data_in,
                      SymsVec & prob_table_out,
                      size_t countmax);

  // returns number of one bits in d. and the median in run_median
  void calculateRunLengths(size_t adc_bits,
				 ADCCountVec const & d, 
				 DataVec & out,
				 unsigned long & run_median,
				 unsigned long & one_bit_total,
				 unsigned long & total_bits);
}

struct ds50::SymProb {
  SymProb() : sym(0), count(0) { }
  explicit SymProb(unsigned int s, unsigned long c = 0UL) : sym(s), count(c) { }

  unsigned int sym;
  unsigned long count;

  void incr() { ++count; }
  bool operator<(SymProb const & other) const 
  { return this->count > other.count; }
};

/* for testing */
inline std::ostream & ds50::operator<<(std::ostream & ost, 
				       SymProb const & s)
{
  ost << "(" << s.sym << "," << s.count << ")";
  return ost;
}

namespace ds50 {
  class ProbCalculator
  {
  public:
    explicit ProbCalculator(size_t bits_adc, bool use_diffs=false);
    void apply(adc_type const* start, adc_type const* end);
    void get(SymsVec& out) const;
  private:
    adc_type mask_;
    size_t countmax_;
    SymsVec syms_;
    bool use_diffs_;
  };
}

// ---------------

#endif /* ds50daq_Compression_SymProb_hh */
