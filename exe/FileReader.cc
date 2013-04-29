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

FileReader::FileReader(FileReader&& fr) :
  name_(fr.name_),
  fragment_(fr.fragment_),
  file_(fr.file_),
  size_in_words_(fr.size_in_words_)
{
  fr.file_ = 0;
}

FileReader& FileReader::operator=(FileReader&& fr)
{
  name_ = std::move(fr.name_);
  fragment_ = fr.fragment_;
  file_ = fr.file_;
  fr.file_ = 0;
  return *this;
}


FileReader::~FileReader()
{
  if (file_ != nullptr) fclose(file_);
}

std::unique_ptr<Fragment>
FileReader::getNext(size_t event_number)
{
  V172xFragment::Header head;
  size_t items = fread(&head, sizeof(head),1,file_);

  if (items==0 && feof(file_)) return nullptr;
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

  std::unique_ptr<Fragment>  result(new Fragment(event_number,
                                                 fragment_,
                                                 ds50::Config::V1720_FRAGMENT_TYPE));
  result->resize(total_word_count);

  memcpy(&(*(result->dataBegin())), &head, sizeof(head));
  void* dest = &*((result->dataBegin()) + head_size_words);
  items = fread(dest, 4, total_int32_to_read, file_);

  if (items==0 && feof(file_)) return nullptr;
  if (items!=total_int32_to_read) 
    throw art::Exception(art::errors::FileReadError)
      << "Failed readin data from: " << name_;

  return result;
}
