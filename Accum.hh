#ifndef ds50_Accum_HH
#define ds50_Accum_HH

#include "ds50daq/Compression/SymTable.hh"
#include "ds50daq/Compression/Properties.hh"

namespace ds50 {
  constexpr auto bits_per_word = sizeof(reg_type) * 8;
  
  struct Accum
  {
  public:
    Accum(DataVec & out, SymTable const & syms);
    
    void put(ADCCountVec::value_type const & value);
    reg_type totalBits() const { return total_; }
    
  private:
    SymTable const & syms_;
    reg_type * curr_word_;
    reg_type * buf_end_;
    reg_type curr_pos_;
    reg_type total_;
  };
  
}


#endif
