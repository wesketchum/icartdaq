
#include "artdaq-demo/Products/CompressedV172x.hh"

#include <cstring>

namespace demo {
  CompressedV172x::CompressedV172x(std::vector<artdaq::Fragment> const & init):
    v172x_headers_(init.size()),
    v172x_metadata_list_(init.size()),
    metadata_present_list_(init.size()),
    compressed_fragments_(init.size()),
    counts_(init.size())
  {
    size_t index = 0;
    std::for_each(init.begin(),
                  init.end(),
    [&index, this](artdaq::Fragment const & frag) {
      memcpy(&v172x_headers_[index],
             &*frag.dataBegin(),
             sizeof(HeaderProxy));
      if (frag.hasMetadata()) {
        metadata_present_list_[index] = true;
        V172xFragment::metadata const * md =
          frag.metadata<V172xFragment::metadata>();
        memcpy(&v172x_metadata_list_[index], md, sizeof(MetadataProxy));
      }
      else {
        metadata_present_list_[index] = false;
      }
      ++index;
    });
  }
}
