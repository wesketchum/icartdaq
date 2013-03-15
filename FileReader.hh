#ifndef ds50daq_DAQ_FileReader_hh
#define ds50daq_DAQ_FileReader_hh

#include <cstddef>
#include <cstdio>
#include <string>

#include "artdaq/DAQdata/Fragment.hh"


class FileReader
{
public:
  // This constructor exists to deal with malformed simulation file,
  // which have their size in bytes; for such files, use
  // size_in_words=false.
  FileReader(std::string const& name, std::size_t fragment_number,
             bool size_in_words = true);
  FileReader(FileReader const&) = delete;
  FileReader& operator=(FileReader const&) = delete;
  bool getNext(artdaq::Fragment& out, size_t event_number);

private:
  std::string const name_;
  std::size_t const fragment_;
  FILE* file_;
  bool const size_in_words_;
};


#endif
