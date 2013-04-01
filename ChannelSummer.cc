#include "darkart/ArtModules/ChannelSummer.hh"

std::unique_ptr<darkart::Channel>
darkart::make_sum(Channels const& channels)
{
  size_t result_size =

    channels.empty() ? 0 : channels.front().size();


  std::unique_ptr<Channel>  result(new Channel(Channel::SUM_CHANNEL_ID,
                                               Channel::INVALID_BOARD_ID,
                                               result_size));
  // The Channel was made with the right *capacity*, but now we have to
  // make its *size* correct.
  result->waveform.resize(result_size);
  for (auto const& ch : channels)
    {
      for (size_t i = 0; i < result_size; ++i)
        {
          result->waveform[i] += ch.waveform[i];
        }
    }

  return result;
}

