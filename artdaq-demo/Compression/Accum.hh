#ifndef artdaq_demo_Compression_Accum_hh
#define artdaq_demo_Compression_Accum_hh

#include "artdaq-demo/Compression/SymTable.hh"
#include "artdaq-demo/Compression/Properties.hh"

namespace demo {
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


#endif /* artdaq_demo_Compression_Accum_hh */
