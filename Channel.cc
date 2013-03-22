#include "darkart/Products/Channel.hh"

darkart::Channel::Channel() :
  channel_id(),
  board_id(),
  waveform()
{ }

darkart::Channel::Channel(int cid, int bid, std::size_t cap) :
  channel_id(cid),
  board_id(bid),
  waveform()
{
  waveform.reserve(cap);
}
