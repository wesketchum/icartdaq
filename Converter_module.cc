////////////////////////////////////////////////////////////////////////
// Class:       Converter
// Module Type: producer
// File:        Converter_module.cc
//
// Generated at Thu Mar 21 20:32:50 2013 by James Kowalkowski using artmod
// from art v1_03_03.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Utilities/Exception.h"
#include "art/Utilities/InputTag.h"

#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq/DAQdata/Fragments.hh"

#include "ds50daq/DAQ/V172xFragment.hh"
#include "ds50daq/DAQ/Config.hh"

#include "darkart/Products/Channel.hh"
#include "darkart/ArtModules/converter_algs.hh"

#include <utility>
#include <memory>

//----------------------------------------------------------------------------
// Class Converter is an EDProducer that creates two products, each of
// type darkart::Channels (a vector of channels) from two inputs: V1720
// and V1724 Fragments.
//----------------------------------------------------------------------------

class Converter : public art::EDProducer {
public:
  explicit Converter(fhicl::ParameterSet const & p);
  virtual ~Converter();
  void produce(art::Event & e) override;

private:
  art::InputTag v1720_tag_;
  art::InputTag v1724_tag_;
};


//----------------------------------------------------------------------------
// The implementation of the module member functions begins here.

Converter::Converter(fhicl::ParameterSet const & ps ) :
  v1720_tag_(ps.get<std::string>("v1720_tag")),
  v1724_tag_(ps.get<std::string>("v1724_tag"))
{
  produces<darkart::Channels>("V1720");
  produces<darkart::Channels>("V1724");
}

Converter::~Converter()
{ }

void Converter::produce(art::Event & e)
{
  // need to get module_label="daq" and instance_name="1720" or "1724"
  art::Handle<artdaq::Fragments> h_1720, h_1724;
  e.getByLabel(v1720_tag_, h_1720);
  e.getByLabel(v1724_tag_, h_1724);

  // Make our products, which begin empty.
  std::unique_ptr<darkart::Channels> v1720(new darkart::Channels);
  std::unique_ptr<darkart::Channels> v1724(new darkart::Channels);

  // Call the algorithm that will fill the products. We have to pass in
  // the enumeration value that tells us the type of board we're dealing
  // with, because the data don't carry that information directly.
  darkart::convert_fragments(*h_1720, *v1720, ds50::Config::V1720_FRAGMENT_TYPE);
  darkart::convert_fragments(*h_1724, *v1724, ds50::Config::V1724_FRAGMENT_TYPE);

  // Put our complete products into the Event.
  e.put(std::move(v1720), "V1720");
  e.put(std::move(v1724), "V1724");
}


DEFINE_ART_MODULE(Converter)
