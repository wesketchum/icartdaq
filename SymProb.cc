
#include <algorithm>
// next ones for debugging
#include <iterator>
#include <iostream>

#include "ds50daq/Compression/SymProb.hh"

using namespace std;

void ds50::calculateProbs(ADCCountVec const & d, SymsVec & out, size_t countmax)
{
  unsigned int symnum = 0;
  out.clear();
  out.reserve(Properties::count_max());
  // generate one slot for possible symbols
  generate_n(back_inserter(out), countmax,
  [&]() { return SymProb(symnum++); });
  for_each(d.cbegin(), d.cend(),
           [&](ADCCountVec::value_type v)
  { if (v < countmax) { out[v].incr(); } });
  // must leave zero count entries in
  for_each(out.begin(), out.end(),
  [](SymsVec::value_type & v) { if (v.count == 0) { v.count = 1; } });
  sort(out.begin(), out.end()); // descending
  // copy(out.begin(),out.end(),ostream_iterator<SymProb>(cout,"\n"));
}

void ds50::calculateRunLengths(size_t adc_bits,
			       ADCCountVec const & d, 
			       DataVec & out,
			       unsigned long & run_median,
			       unsigned long & one_bits,
			       unsigned long & total_bits)
{
  DataVec runs;
  runs.reserve(1000);
  unsigned long bit_count=0;

  one_bits = 0;
  total_bits = 0;

  for (auto b = d.cbegin(), e = d.cend(); b != e; ++b) 
    {
      auto curr = *b;
      for (size_t i = 0; i < adc_bits; ++i) 
	{
	  ++total_bits;
	  if ((curr & 0x01)==0) 
	    {
	      ++one_bits;
	      if (out.size() <= bit_count)
		{ out.resize(bit_count + 1); }
	      ++out[bit_count];
	      runs.push_back(bit_count);
	      bit_count = 0;
	    }
	  else
	    { ++bit_count; }
	  curr >>= 1;
	}
    }
  
  sort(runs.begin(), runs.end()); // ascending
  size_t num_runs = runs.size();
  run_median = (num_runs % 2) == 1 ?
    runs[num_runs / 2] : 
    (runs[num_runs / 2 - 1] + runs[num_runs / 2]) / 2;
  // sort(out.begin(), out.end()); // descending
}
