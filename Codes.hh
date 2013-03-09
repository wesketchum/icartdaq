#ifndef ds50daq_Codes_HH
#define ds50daq_Codes_HH

#include "ds50daq/Compression/SymTable.hh"
#include "ds50daq/Compression/Properties.hh"

namespace ds50
{

  typedef unsigned long code_type;

  struct Code {
    code_type value_ : 32;
    code_type length_ : 32;
  };

  code_type unary_code(long n);
  code_type b_mask(long b);

  Code pod(code_type n);

  template <code_type k>
  Code subexp(code_type n)
  {
    constexpr code_type a = 1 << k;
    code_type b = n < a ? k : floor(log2(n));
    code_type u = n < a ? 0 : b - k + 1;
    code_type left = unary_code(u) << b; // coded in u+1 bits
    code_type right = b_mask(b) & n;
    Code result;
    // number of bits in result code = u+1+b
    result.value_ = left | right;
    result.length_ = u + 1 + b;
    return result;
  }

  void generateTable(Code (*)(code_type), SymTable& out, size_t total);
  unsigned long rleAndCompress(ADCCountVec const& in, DataVec& out, SymTable const&);
  unsigned long rleAndCompress(ADCCountVec::const_iterator& in_start, ADCCountVec::const_iterator& in_end,
			       DataVec& out, SymTable const&);
  
}

#endif
