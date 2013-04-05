
#include <algorithm>
// next ones for debugging
#include <iterator>
#include <iostream>

#include "ds50daq/Compression/SymProb.hh"

using namespace std;

void ds50::calculateProbs(ADCCountVec const & d,
			  SymsVec & out, 
			  size_t countmax)
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

namespace ds50 {
  ProbCalculator::ProbCalculator(size_t bits_adc, bool use_diffs):
    mask_(use_diffs==false ? (1<<bits_adc)-1 : (1<<16)-1),
    countmax_(mask_),
    syms_(),
    use_diffs_(use_diffs)
  {
    syms_.reserve(countmax_);
    unsigned int symnum=0;
    
    generate_n(back_inserter(syms_), countmax_+1,
	       [&]() { return SymProb(symnum++,1); });
  }
  
  void ProbCalculator::apply(adc_type const* start, adc_type const* end)
  {
    if(!use_diffs_)
      {
	for_each(start, end,
		 [&](adc_type v)
		 { syms_[v&mask_].incr(); });
      }
    else
      {
	if(end==start) return;
	if(*start<countmax_) syms_[*start].incr();

	for(;(start+1)!=end;++start)
	  {
	    // jbk - warning, this calculation need to be put 
	    // into one place
	    adc_type v1 = *start&mask_, v2=*(start+1)&mask_;
	    adc_type v3 = v2-v1;
	    assert((size_t)(v3)<=countmax_);
	    syms_[v3].incr();
	  }
      }
  }
  
  void ProbCalculator::get(SymsVec& out) const
  {
    // copy and sort correctly
    out = syms_;
    sort(out.begin(),out.end());
  }
}

// -----------------

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
