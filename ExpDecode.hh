#ifndef ds50daq_expdecode_HH
#define ds50daq_expdecode_HH

#include "ds50daq/Compression/Properties.hh"

namespace ds50 {
  struct NextBit
  {
    NextBit(reg_type const* in, reg_type bit_count):
      bit_count_(bit_count),
      in_(in),
      index_(0),
      curr_(in_[index_]),
      pos_(0)
    { }

    bool done() const
    {
      return bit_count_==0;
      // return (pos_ * sizeof(reg_type) * 8) == bit_count;
    }

    adc_type next()
    {
      if(pos_>=sizeof(reg_type)*8)
	{
	  ++index_;
	  curr_=in_[index_];
	  pos_=0;
	}

      reg_type v = curr_ & 0x0001;
      ++pos_;
      curr_>>=1;

      --bit_count_;
      return v;
    }

    reg_type bit_count_;
    reg_type const* in_;
    size_t index_;
    reg_type curr_;
    size_t pos_;
  };

  struct SaveBit
  {
    SaveBit(size_t adc_bits, adc_type* out):
      adc_bits_(adc_bits),
      out_(out),
      curr_(out),
      pos_(0) // pos_(adc_bits_)
    {
      *curr_=0;
    }

    void pushZeros(adc_type v)
    {
      for(int i=0;i<v;++i) pushBit(0);
    }

    void push(adc_type v)
    {
      // put v zeros followed by a 1
      pushZeros(v);
      pushBit(1);
    }

    void pushBit(adc_type b)
    {
      if(pos_==adc_bits_)
	{
	  ++curr_;
	  *curr_ = 0;
	  pos_=0;
	  // jbk - old way when arg was vector
	  // out_.push_back(0);
	  // curr_=&out_.back();
	}

      *curr_ |= ((b&0x0001)<<pos_);
      // *curr_ = (*curr_<<1) | b;
      pos_+=1;

    }

    size_t adc_bits_;
    adc_type* out_;
    adc_type* curr_;
    adc_type pos_;
  };

  reg_type decodePod(size_t adc_bits,
		     reg_type bit_count,
		     DataVec const& in,
		     ADCCountVec& out);

  reg_type decodePod(size_t adc_bits,
		     reg_type bit_count,
		     reg_type const* in,
		     adc_type* out);
}

#endif

