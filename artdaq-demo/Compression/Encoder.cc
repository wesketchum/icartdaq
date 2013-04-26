
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

Encoder::Encoder(SymTable const & syms, bool use_diffs):
  syms_(syms),
  use_diffs_(use_diffs)
{
  reverseCodes(syms_);
}

reg_type Encoder::operator()(ADCCountVec const & in, DataVec & out)
{
  return (*this)(&in[0], &in[in.size()] , out);
}

reg_type Encoder::operator()(adc_type const * beg, 
			     adc_type const * end, 
			     DataVec & out)
{
  Accum a(out, syms_);

  if(use_diffs_)
    {
      if(end==beg) return 0;
      a.put(*beg);

      for(;(beg+1)!=end;++beg)
	  {
	    // jbk - warning, this calculation need to be put 
	    // into one place (see SymProb.cc and for the other)
	    adc_type v1 = *beg, v2=*(beg+1);
	    adc_type v3 = v2-v1;
	    a.put(v3);
	  }
    }
  else
    {
      for_each(beg, end, [&](adc_type const & v) { a.put(v); });
    }
  return a.totalBits();
}

