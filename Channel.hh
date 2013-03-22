#ifndef darkart_Products_Channel_hh
#define darkart_Products_Channel_hh

#include <vector>

namespace darkart
{
  struct Channel
  {
    // Default constructed Channel has channel_id and board_id that are
    // invalid (0), and an empty waveform.
    Channel();

    // Create a Channel object with the given channel_id and board_id,
    // and with a waveform with capacity (not size!) of 'cap'.
    Channel(int cid, int bid, std::size_t cap);

    // Channel is a struct, because there are no class invariants that
    // need to be maintained; it is a simple 'value type'.
    int channel_id;
    int board_id;
    std::vector<double> waveform;
  };

  // The event data product we store is a vector of channels.
  typedef std::vector<Channel> Channels;
}

#endif
