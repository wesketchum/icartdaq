////////////////////////////////////////////////////////////////////////
// Class:       Summer
// Module Type: producer
// File:        Summer_module.cc
//
// Generated at Thu Mar 28 12:03:09 2013 by Marc Paterno using artmod
// from art v1_03_08.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Utilities/InputTag.h"

#include <memory>
#include "fhiclcpp/ParameterSet.h"

#include "darkart/Products/Channel.hh"
#include "darkart/ArtModules/ChannelSummer.hh"

//----------------------------------------------------------------------------
// Class Summer is an EDProducer that creates two products, each of
// which is a darkart::Channel (not a vector, a single Channel) formed
// from a vector of Channels; each produced Channel contains the sum of
// the values of the input Channels.
//----------------------------------------------------------------------------

class Summer : public art::EDProducer {
public:
  explicit Summer(fhicl::ParameterSet const & p);
  virtual ~Summer();
  void produce(art::Event & e) override;
private:
  art::InputTag v1720_tag_;
  art::InputTag v1724_tag_;
};

using darkart::Channel;
using darkart::Channels;


//----------------------------------------------------------------------------
// The implementation of the module member functions begins here.


Summer::Summer(fhicl::ParameterSet const & ps) :
  v1720_tag_(ps.get<std::string>("v1720_tag")),
  v1724_tag_(ps.get<std::string>("v1724_tag"))
{
  produces<Channel>("V1720");
  produces<Channel>("V1724");
}

void Summer::produce(art::Event & e)
{
  // Get the inputs
  art::Handle<Channels> h_v1720, h_v1724;
  e.getByLabel(v1720_tag_, h_v1720);
  e.getByLabel(v1724_tag_, h_v1724);

  // Sum the channels. The function we call returns the result *by
  // value*, i.e. it is copied. This is not expensive because the type
  // in question has an inexpensive copy (a "move copy"). We put the
  // unnamed product directly into the event.
  e.put(make_sum(*h_v1720), "V1720");
  e.put(make_sum(*h_v1724), "V1724");
}

Summer::~Summer()
{ }

DEFINE_ART_MODULE(Summer)
