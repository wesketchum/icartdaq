#include "ds50daq/DAQ/FileReader.hh"
#include "art/Utilities/Exception.h"

#include <stdexcept>

#include "ds50daq/DAQ/V172xFragment.hh"
#include "ds50daq/DAQ/Config.hh"

using namespace std;
using artdaq::Fragment;
using ds50::V172xFragment;

FileReader::FileReader(string const& name,
                       size_t fragment_number,
                       bool size_in_words) :
  name_(name),
  fragment_(fragment_number),
  file_(fopen(name.c_str(),"rb")),
  size_in_words_(size_in_words)
{
  if (!file_) 
    throw art::Exception(art::errors::FileOpenError)
      << "Unable to open file: " << name << endl;
}


bool FileReader::getNext(Fragment& out, size_t event_number)
{
  V172xFragment::Header head;
  size_t items = fread(&head, sizeof(head),1,file_);

  if (items==0 && feof(file_)) return false;
  if (items!=1) 
    throw art::Exception(art::errors::FileReadError)
      << "Failed reading header from: " << name_;

  // Deal with incorrectly formatted simulation file.
  if (!size_in_words_)     // file is incorrectly formatted
    head.event_size /= 4;  // turn size-in-bytes to size-in-32-bit-integers

  size_t head_size_words = sizeof(head) / sizeof(Fragment::value_type);
  size_t total_word_count =
    ceil(head.event_size*V172xFragment::Header::size_words 
				 / (double)sizeof(Fragment::value_type));


  size_t total_int32_to_read = head.event_size - sizeof(head)/4;

  Fragment frag(event_number, fragment_, ds50::Config::V1720_FRAGMENT_TYPE);
  frag.resize(total_word_count);

  memcpy(&(*frag.dataBegin()), &head, sizeof(head));
  //auto dest = frag.dataBegin() + head_size_words;
  void* dest = &*(frag.dataBegin() + head_size_words);
  items = fread(dest, 4, total_int32_to_read, file_);

  if (items==0 && feof(file_)) return false;
  if (items!=total_int32_to_read) 
    throw art::Exception(art::errors::FileReadError)
      << "Failed readin data from: " << name_;

  out = frag;
  return true;
}
