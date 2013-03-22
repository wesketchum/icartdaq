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
#include "artdaq/DAQdata/Fragment.hh"
#include "artdaq/DAQdata/Fragments.hh"

class Converter;

class Converter : public art::EDProducer {
public:
  explicit Converter(fhicl::ParameterSet const & p);
  virtual ~Converter();

  void produce(art::Event & e) override;

private:
  // Declare member data here.

};

Converter::Converter(fhicl::ParameterSet const & )
// :
// Initialize member data here.
{
  // Call appropriate Produces<>() functions here.
}

Converter::~Converter()
{
  // Clean up dynamic memory and other resources here.
}

void Converter::produce(art::Event & e)
{
  // need to get module_label="daq" and instance_name="1720" or "1724"
  art::Handle<artdaq::Fragments> h_1720, h_1724;
  e.getByLabel("daq", h_1720, "1720");
  e.getByLabel("daq", h_1724, "1724");

  // std::unique_ptr<CompressedEvent> prod(new CompressedEvent(*handle));

  size_t const len = h_1720->size();
  for (size_t i = 0; i < len; ++i) 
    {
      artdaq::Fragment const & frag = (*h_1720)[i];
      V172xFragment b(frag);
      // start of payload is the DS50 header
      auto adc_start = b.dataBegin();
      auto adc_end   = b.dataEnd();

    }
    
  // e.put(std::move(prod));
}

DEFINE_ART_MODULE(Converter)
