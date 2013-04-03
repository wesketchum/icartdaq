
#include "ds50daq/Compression/ExpDecode.hh"

#include <iostream>
using namespace std;

namespace ds50 {

  // jbk - warning ...
  // this code has not been reduced to simplest form

  reg_type decodePod(size_t adc_bits,
		     reg_type bit_count,
		     DataVec const& in,
		     ADCCountVec& out)
  {
    return decodePod(adc_bits, bit_count,
		     &in[0], &out[0]);
		     
  }

  reg_type decodePod(size_t adc_bits,
		     reg_type bit_count,
		     reg_type const* in,
		     adc_type* out)
  {
    // ADCCountVec tmp;
    NextBit nb(in, bit_count);
    SaveBit sb(adc_bits, out);
    size_t num_zeros=0;

    // std::cout << "------ bit count = " << bit_count << "\n";
    bool no_ones=true;

    while(!nb.done())
      {
	adc_type v = nb.next();

	if(v)
	  {
	    // pull num_zeros bits from nb
	    if(num_zeros==0) 
	      { 
		// std::cout << " num_zeros=" << num_zeros << "\n";
		// sb.pushBit(1);
	      }
	    else
	      {
		for(unsigned i=1;i<num_zeros;++i)
		  v = (v<<1)|nb.next();
		sb.pushZeros(v);
		// std::cout << "v=" << v << " num_zeros=" << num_zeros << "\n";
	      }
	    if(!nb.done() ||  no_ones || (nb.done() && num_zeros==0 && !no_ones))
	      {
		sb.pushBit(1); 
		no_ones=false;
		// cout << "pushed 1\n";
	      }
	    num_zeros=0;
	  }
	else
	  ++num_zeros;
      }

    if(num_zeros>0) 
      {
	cout << "got final zeros = " << num_zeros << "\n";
	sb.pushZeros(num_zeros-1);
      }
    return 0;
  }
  
}
