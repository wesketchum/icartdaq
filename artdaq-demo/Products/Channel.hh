#ifndef darkart_Products_Channel_hh
#define darkart_Products_Channel_hh

#include <vector>

namespace darkart
{
  // An instance of struct Channel represents the number of
  // photoelectrons per sample in the raw data. A channel id of 0 is
  // invalid, and is only used for default-constructed Channels;
  // similarly for board id of 0.
  struct Channel
  {
    static const int INVALID_CHANNEL_ID = 0;
    static const int INVALID_BOARD_ID = 0;
    static const int SUM_CHANNEL_ID = -1;
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

    // As a matter of course, we expose as little of our product
    // interface to Root as possible. This helps minimize the size of
    // the created dictionaries.

    // Return the number of samples in the waveform.
    std::size_t size() const;

    // Increase the extent of the contained vector of samples to have
    // capacity (not size) cap.
    void reserve(std::size_t cap);

    // Add a new sample
    void add(double x);
  };

  // Channels are value types, so two Channels are equal if all their
  // data are equal.
  inline
  bool operator==(Channel const& a, Channel const& b)
  {
    return

      a.channel_id == b.channel_id &&
      a.board_id   == b.board_id   &&
      a.waveform   == b.waveform;
  }

  // The event data product we store is a vector of channels.
  typedef std::vector<Channel> Channels;
}


inline
std::size_t
darkart::Channel::size() const
{
  return waveform.size();
}

inline
void
darkart::Channel::reserve(std::size_t cap)
{
  waveform.reserve(cap);
}

inline
void
darkart::Channel::add(double x)
{
  waveform.push_back(x);
}

#endif
