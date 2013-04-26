#ifndef ds50daq_DAQ_CompressedV172x_hh
#define ds50daq_DAQ_CompressedV172x_hh

// NOTE: the GPU might be more efficient at using 32-bit integers than 64-bit integers,
// in this case the code below will need to be modified.

// What size do we want the compressed vectors? resized to the amount of data.
// Should we also store a vector of compressed fragment lengths? yes, because the
// total bits returned from the encoders is an important number

#include "artdaq/DAQdata/features.hh"
#include "artdaq/DAQdata/Fragment.hh"
#include "ds50daq/DAQ/V172xFragment.hh"

#include <vector>

namespace ds50 {
  class CompressedV172x;
}

class ds50::CompressedV172x {
public:
  typedef uint64_t reg_type;
  typedef std::vector<reg_type> DataVec;
  enum Algo_t { AlgoHuffman, AlgoPod, AlgoSubexp0, AlgoSubexp1, AlgoUnknown };

  CompressedV172x() { }
  // will set up the headers and the sizes given a set of fragments
  explicit CompressedV172x(std::vector<artdaq::Fragment> const & init);

  DataVec & fragment(size_t which)
    { return compressed_fragments_.at(which); }
  DataVec const & fragment(size_t which) const
    { return compressed_fragments_.at(which); }

  reg_type fragmentBitCount(size_t which) const
    { return counts_.at(which); }
  void setFragmentBitCount(size_t which, reg_type count)
    { counts_[which] = count; }

  size_t size() const { return compressed_fragments_.size(); }

  // return a reference to the entire CompVec? perhaps.

#if USE_MODERN_FEATURES
  // since structures for headers are in the details, there is
  // no other clean way to present them here to the user.
  artdaq::Fragment headerOnlyFrag(size_t which) const;
#endif

  // Needs to be public for ROOT persistency: do not use.
  struct HeaderProxy {
    V172xFragment::Header::data_t hp[V172xFragment::Header::size_words];
  };
  struct MetadataProxy {
    V172xFragment::metadata::data_t hp[V172xFragment::metadata::size_words];
  };

  void setMetadata(size_t bits, Algo_t algo, int bias=0)
  { bias_=bias; bits_=bits, algo_=algo; }
  size_t getAdcBits() const { return bits_; }
  int getBias() const { return bias_; }
  Algo_t getAlgo() const { return algo_; }

private:
  // ROOT persistency can't handle bitfields.
  typedef std::vector<HeaderProxy> V172xHeaderVec;
  typedef std::vector<MetadataProxy> V172xMetadataVec;
  typedef std::vector<bool> MetadataPresentVec;
  typedef std::vector<DataVec> CompVec;
  typedef std::vector<reg_type> CountVec;

  V172xHeaderVec v172x_headers_;
  V172xMetadataVec v172x_metadata_list_;
  MetadataPresentVec metadata_present_list_;
  CompVec compressed_fragments_;
  CountVec counts_;

  // need some metadata: ADC bits, Compression algo name, algo bias
  int bias_;
  size_t bits_;
  Algo_t algo_;
};

#if USE_MODERN_FEATURES

inline
artdaq::Fragment
ds50::CompressedV172x::headerOnlyFrag(size_t which) const
{
  using artdaq::Fragment;
  Fragment result
    (Fragment::dataFrag(Fragment::InvalidSequenceID,
                        Fragment::InvalidFragmentID,
                        reinterpret_cast<Fragment::value_type const *>
                        (&v172x_headers_.at(which)),
                        std::ceil((V172xFragment::Header::size_words *
                                   sizeof(V172xFragment::Header::data_t)) /
                                  sizeof(artdaq::RawDataType))));
  ds50::V172xFragment b(result);
  result.setSequenceID(b.event_counter());
  result.setFragmentID(b.board_id());
  if (metadata_present_list_[which]) {
    V172xFragment::metadata const * mdPtr =
      reinterpret_cast<V172xFragment::metadata const *>
      (&v172x_metadata_list_.at(which));
    result.setMetadata(*mdPtr);
  }
  return result;
}
#endif /* USE_MODERN_FEATURES */

#endif /* ds50daq_DAQ_CompressedV172x_hh */
