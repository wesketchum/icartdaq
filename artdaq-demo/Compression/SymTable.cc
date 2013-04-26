#include "artdaq-demo/Compression/SymTable.hh"
#include "artdaq-demo/Compression/Properties.hh"
#include "cetlib/exception.h"

#include <fstream>
#include <algorithm>
#include <iterator>

using namespace std;
using namespace demo;

void demo::readTable(const char * fname, SymTable & out, size_t countmax)
{
  std::ifstream ifs(fname);
  if (!ifs) 
    {
      throw cet::exception("FileOpenError")
	<< "Unable to open file "
	<< fname;
    }

  out.clear();
  out.reserve(countmax);

  // copy(istream_iterator<TableEntry>(ifs),istream_iterator<TableEntry>(),
  //  back_inserter(from_file));

  while (1) 
    {
      SymCode te;
      ifs >> te;
      if (ifs.eof() || ifs.bad()) { break; }
      out.push_back(te);
    }

  std::sort(out.begin(),out.end(),
	    [](SymCode const& a, SymCode const& b)
	    { return a.sym_ < b.sym_; });
}

void demo::writeTable(const char * fname, SymTable const & in)
{
  ofstream ofs(fname);
  copy(in.cbegin(), in.cend(), ostream_iterator<SymCode>(ofs, "\n"));
}

void demo::reverseCodes(SymTable & out)
{
  // reverse the bits
  for (auto cur = out.begin(), end = out.end(); cur != end; ++cur) {
    reg_type in_reg  = cur->code_;
    reg_type out_reg = 0;
    for (reg_type i = 0; i < cur->bit_count_; ++i) { out_reg <<= 1; out_reg |= (in_reg >> i) & 0x01; }
    cur->code_ = out_reg;
  }
}
