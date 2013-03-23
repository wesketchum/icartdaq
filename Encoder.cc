
#include "ds50daq/Compression/Encoder.hh"
#include "ds50daq/Compression/Accum.hh"
#include "cetlib/exception.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace ds50;


// -----------------

Encoder::Encoder(SymTable const & syms):
  syms_(syms)
{
  reverseCodes(syms_);
}

reg_type Encoder::operator()(ADCCountVec const & in, DataVec & out)
{
  return (*this)(&in[0], &in[in.size()] , out);
}

reg_type Encoder::operator()(adc_type const * beg, adc_type const * end, DataVec & out)
{
  Accum a(out, syms_);
  for_each(beg, end, [&](adc_type const & v) { a.put(v); });
  return a.totalBits();
}

