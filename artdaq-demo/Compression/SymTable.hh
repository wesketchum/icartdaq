#ifndef ds50daq_Compression_SymTable_hh
#define ds50daq_Compression_SymTable_hh

#include "ds50daq/Compression/SymCode.hh"
#include <vector>

namespace ds50 {
  typedef std::vector<SymCode> SymTable;

  void readTable(const char * fname, SymTable & out, size_t countmax);
  void writeTable(const char * fname, SymTable const & in);
  void reverseCodes(SymTable &);
}

#endif /* ds50daq_Compression_SymTable_hh */
