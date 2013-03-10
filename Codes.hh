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

  // decoding pod is straightfoward, see the cc file for details

  Code pod(code_type n);

  // decoding subexp0 is straightforward: look for a string of 1s, the
  // length of the string is the length of the number afterwards that
  // needs to be decoded. To calculate the run length, use the following
  // formula: 2^(num_of_1s-1) + num_after_the_1s
  // in other words, if 10 bits are extracted, including the beginning 1s,
  // use 2^(10/2-1) + value&0x000F

  // decoding subexp1 is similar, except look for length+1 for the
  // size of the number to decode after the string of 1s.
  // formula: 2^(num_of_1s) + num_after_the_1s
  // in other words, if 11 bits are extracted, including the 1s,
  // use 2^5 + value&001F

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

  // this function will work for each of the encoding methods in this file, you need to
  // know the specific encoding method to decode it. returns the number of bits.
  unsigned long rleAndCompress(ADCCountVec const& in, DataVec& out, SymTable const&, unsigned bias);
  unsigned long rleAndCompress(ADCCountVec::value_type const* in_start, ADCCountVec::value_type const* in_end,
			       DataVec& out, SymTable const&, unsigned bias);
  
  unsigned long decodePod(DataVec const& source, ADCCountVec& destination, unsigned bias);
  unsigned long decodeSubexp(DataVec const& source, ADCCountVec& destination, unsigned bias, unsigned k);
}

#endif
