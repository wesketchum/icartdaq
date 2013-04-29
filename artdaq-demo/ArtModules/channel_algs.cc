#include "darkart/ArtModules/channel_algs.hh"

#include <numeric>



// Average will return NaN if called on an empty range (b == e).
inline
double average(std::vector<double>::const_iterator b, std::vector<double>::const_iterator e)
{
  return std::accumulate(b, e, 0.0)/(e-b);
}

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

std::unique_ptr<darkart::Channel>
darkart::make_baseline_subtracted(Channel const& ch, std::size_t numavg)
{
  // Averaging 0 elements has to be dealt with specially, because
  // average() returns NaN on an empty range.
  if (numavg == 0) return std::unique_ptr<Channel>(new Channel(ch));

  auto b = ch.waveform.begin();
  double avg = average(b, b+numavg);
  std::unique_ptr<Channel> result(new Channel(ch.channel_id, ch.board_id, ch.size()));
  for (auto v : ch.waveform) result->waveform.push_back(v-avg);
  return result;
}
