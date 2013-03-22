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

void Converter::produce(art::Event & )
{
  // Implementation of required member function here.
}

DEFINE_ART_MODULE(Converter)
