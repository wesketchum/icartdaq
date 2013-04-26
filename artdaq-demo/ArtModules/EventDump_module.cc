////////////////////////////////////////////////////////////////////////
// Class:       EventDump
// Module Type: analyzer
// File:        EventDump_module.cc
// Description: Prints out information about each event.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"

#include "art/Utilities/Exception.h"
#include "ds50daq/DAQ/V172xFragment.hh"
#include "ds50daq/DAQ/V1495Fragment.hh"
#include "ds50daq/DAQ/V1190Fragment.hh"
#include "artdaq/DAQdata/Fragments.hh"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <vector>
#include <iostream>

namespace ds50 {
  class EventDump;
}

class ds50::EventDump : public art::EDAnalyzer {
public:
  explicit EventDump(fhicl::ParameterSet const & pset);
  virtual ~EventDump();

  virtual void analyze(art::Event const & evt);

private:
  std::string raw_data_label_;
  std::string uncompressed_V1720_label_;
  std::string uncompressed_V1724_label_;
};


ds50::EventDump::DS50EventDump(fhicl::ParameterSet const & pset) :
  raw_data_label_(pset.get<std::string>("raw_data_label")),
  uncompressed_V1720_label_(pset.get<std::string>("uncompressed_V1720_label")),
  uncompressed_V1724_label_(pset.get<std::string>("uncompressed_V1724_label"))
{
}

ds50::EventDump::~DS50EventDump()
{
}

void ds50::EventDump::analyze(art::Event const & evt)
{
  art::EventNumber_t eventNumber = evt.event();

  // ***********************
  // *** V1720 Fragments ***
  // ***********************

  // look for raw V1720 data first, but also check for uncompressed
  // V1720 data
  art::Handle<artdaq::Fragments> raw;
  evt.getByLabel(raw_data_label_, "V1720", raw);
  if (! raw.isValid()) {
    evt.getByLabel(uncompressed_V1720_label_, "V1720", raw);
  }

  if (raw.isValid()) {
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has " << raw->size()
              << " V1720 fragment(s)." << std::endl;

    for (size_t idx = 0; idx < raw->size(); ++idx) {
      const auto& frag((*raw)[idx]);

      std::cout << "  Fragment " << frag.fragmentID() << " has size "
                << frag.size() << " words." << std::endl;

      V172xFragment bb(frag);
      size_t board_id = bb.board_id ();
      std::cout << "    board ID " << board_id << ", data size = "
                << bb.event_size() << ", total ADC values = "
                << bb.total_adc_values() << ", event counter = "
                << bb.event_counter() << "." << std::endl;

      if (frag.hasMetadata()) {
        V172xFragment::metadata const* md =
          frag.metadata<V172xFragment::metadata>();
        std::cout << std::showbase << "    model number = "
                  << ((int)md->v17xx_model) << ", sample bits = "
                  << ((int)md->sample_bits) << ", sample rate = "
                  << ((int)md->sample_rate_MHz) << " MHz"
                  << std::endl;
      }
    }
  }
  else {
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has zero"
              << " V1720 fragments." << std::endl;
  }
  std::cout << std::endl;

  // ***********************
  // *** V1724 Fragments ***
  // ***********************

  // look for raw V1724 data first, but also check for uncompressed
  // V1724 data
  evt.getByLabel(raw_data_label_, "V1724", raw);
  if (! raw.isValid()) {
    evt.getByLabel(uncompressed_V1724_label_, "V1724", raw);
  }

  if (raw.isValid()) {
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has " << raw->size()
              << " V1724 fragment(s)." << std::endl;

    for (size_t idx = 0; idx < raw->size(); ++idx) {
      const auto& frag((*raw)[idx]);

      std::cout << "  Fragment " << frag.fragmentID() << " has size "
                << frag.size() << " words." << std::endl;

      V172xFragment bb(frag);
      size_t board_id = bb.board_id ();
      std::cout << "    board ID " << board_id << ", data size = "
                << bb.event_size() << ", total ADC values = "
                << bb.total_adc_values() << ", event counter = "
                << bb.event_counter() << "." << std::endl;

      if (frag.hasMetadata()) {
        V172xFragment::metadata const* md =
          frag.metadata<V172xFragment::metadata>();
        std::cout << std::showbase << "    model number = "
                  << ((int)md->v17xx_model) << ", sample bits = "
                  << ((int)md->sample_bits) << ", sample rate = "
                  << ((int)md->sample_rate_MHz) << " MHz"
                  << std::endl;
      }
    }
  }
  else {
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has zero"
              << " V1724 fragments." << std::endl;
  }
  std::cout << std::endl;

  // ***********************
  // *** V1190 Fragments ***
  // ***********************

  evt.getByLabel(raw_data_label_, "V1190", raw);

  if (raw.isValid()) {
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has " << raw->size()
              << " V1190 fragment(s)." << std::endl;

    for (size_t idx = 0; idx < raw->size(); ++idx) {
      const auto& frag((*raw)[idx]);

      std::cout << "  Fragment " << frag.fragmentID() << " has size "
                << frag.size() << " words." << std::endl;

      V1190Fragment bb(frag);
      V1190Fragment::Word const *dataWord = bb.dataBegin();
      if (dataWord != 0) {
        V1190Fragment::Word::event_count_t event_count = 0;
        while (dataWord != bb.dataEnd()) {
          //std::cout << "V1190 " << (*dataWord) << std::endl;
          if (dataWord->type() == V1190Fragment::Word::global_header) {
            event_count = dataWord->event_count();
          }
          ++dataWord;
        }
        std::cout << std::showbase
                  << "    event count = " << event_count
                  << std::endl;
      }
    }
  }
  else {
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has zero"
              << " V1190 fragments." << std::endl;
  }
  std::cout << std::endl;

  // ***********************
  // *** V1495 Fragments ***
  // ***********************

  evt.getByLabel(raw_data_label_, "V1495", raw);

  if (raw.isValid()) {
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has " << raw->size()
              << " V1495 fragment(s)." << std::endl;

    for (size_t idx = 0; idx < raw->size(); ++idx) {
      const auto& frag((*raw)[idx]);

      std::cout << "  Fragment " << frag.fragmentID() << " has size "
                << frag.size() << " words." << std::endl;

      V1495Fragment bb(frag);
      std::cout << std::showbase
                << "    trigger type = " << bb.trigger_type ()
                << ", trigger counter = " << bb.trigger_counter()
                << ", run number = " << bb.run_number()
                << ", GPS coarse = " << bb.gps_coarse()
                << ", GPS fine = " << bb.gps_fine()
                << std::endl;

      if (frag.hasMetadata()) {
        V1495Fragment::metadata const* md =
          frag.metadata<V1495Fragment::metadata>();
        std::cout << std::showbase << "    firmware version = "
                  << ((int)md->firmware_version) << std::endl;
      }
    }
  }
  else {
    std::cout << "Run " << evt.run() << ", subrun " << evt.subRun()
              << ", event " << eventNumber << " has zero"
              << " V1495 fragments." << std::endl;
  }
  std::cout << std::endl;
}

DEFINE_ART_MODULE(ds50::EventDump)
