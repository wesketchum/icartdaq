#ifndef artdaq_demo_Compression_SymTable_hh
#define artdaq_demo_Compression_SymTable_hh

#include "artdaq-demo/Compression/SymCode.hh"
#include <vector>

namespace demo {
  typedef std::vector<SymCode> SymTable;

  void readTable(const char * fname, SymTable & out, size_t countmax);
  void writeTable(const char * fname, SymTable const & in);
  void reverseCodes(SymTable &);
}

#endif /* artdaq_demo_Compression_SymTable_hh */
