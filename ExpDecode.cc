
#include "ds50daq/Compression/ExpDecode.hh"

using namespace std;

namespace ds50 {

  reg_type decodePod(size_t adc_bits,
		     reg_type bit_count,
		     DataVec const& in,
		     ADCCountVec& out)
  {
    // ADCCountVec tmp;
    NextBit nb(in, bit_count);
    SaveBit sb(adc_bits, out);
    size_t num_zeros=0;

    while(!nb.done())
      {
	adc_type v = nb.next();

	if(v)
	  {
	    // pull num_zeros bits from nb
	    if(num_zeros==0)
	      sb.push(0);
	    else
	      {
		while((--num_zeros)>0)
		  v = (v<<1) & nb.next();
		sb.push(v);
	      }
	  }
	else
	  ++num_zeros;
      }
  }
}
