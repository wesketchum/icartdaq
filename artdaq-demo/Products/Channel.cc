#include "artdaq-demo/Products/Channel.hh"

darkart::Channel::Channel() :
  channel_id(INVALID_CHANNEL_ID),
  board_id(INVALID_BOARD_ID),
  waveform()
{ }

darkart::Channel::Channel(int cid, int bid, std::size_t cap) :
  channel_id(cid),
  board_id(bid),
  waveform()
{
  waveform.reserve(cap);
}
