#ifndef ds50daq_DAQ_FileReader_hh
#define ds50daq_DAQ_FileReader_hh

#include <cstddef>
#include <cstdio>
#include <memory>
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

  FileReader(FileReader&& fr);
  FileReader& operator=(FileReader&& fr);

  ~FileReader();

  // Returns a null pointer when the file is exhausted.
  std::unique_ptr<artdaq::Fragment> getNext(size_t event_number);

private:
  std::string name_;
  std::size_t fragment_;
  FILE* file_;
  bool size_in_words_;
};


#endif
