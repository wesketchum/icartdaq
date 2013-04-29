#ifndef artdaq_demo_ArtModules_channel_algs_hh
#define artdaq_demo_ArtModules_channel_algs_hh

#include <memory>

#include "artdaq-demo/Products/Channel.hh"

namespace darkart
{
  // Make a single Channel representing the sample-by-sample sum of the
  // input Channels.
  std::unique_ptr<Channel> make_sum(Channels const& channels);

  // Make a baseline subtracted Channel by calculating the average of
  // the first 'numavg' samples, and subtracting that average from all
  // samples.
  std::unique_ptr<Channel> make_baseline_subtracted(Channel const& channel,
                                                    std::size_t numavg);
}

#endif /* artdaq_demo_ArtModules_channel_algs_hh */
