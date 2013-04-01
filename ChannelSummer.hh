#ifndef darkart_ArtModules_ChannelSummer_hh
#define darkart_ArtModules_ChannelSummer_hh

#include <memory>

#include "darkart/Products/Channel.hh"

namespace darkart
{
  std::unique_ptr<Channel> make_sum(Channels const& channels);
}

#endif
