////////////////////////////////////////////////////////////////////////
// Class:       WFViewer
// Module Type: analyzer
// File:        WFViewer_module.cc
//
// Generated at Sun Dec  2 12:23:06 2012 by Alessandro Razeto & Nicola Rossi using artmod
// from art v1_02_04.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Utilities/InputTag.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "artdaq/DAQdata/Fragments.hh"
#include "artdaq-demo/Overlays/V172xFragment.hh"

#include "TRootCanvas.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TH1D.h"
#include "TStyle.h"

#include <numeric>
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;

namespace demo {
  class WFViewer : public art::EDAnalyzer {
    public:
      explicit WFViewer (fhicl::ParameterSet const & p);
      virtual ~WFViewer ();

      void analyze (art::Event const & e) override;
      void beginRun(art::Run const &) override;

    private:
      std::string board_type_;
      std::unique_ptr<TCanvas> canvas_[2];
      std::vector<std::unique_ptr<TGraph>> sum_graphs_;
      std::vector<Double_t> x_;
      std::vector<bool> normals_;
      int prescale_;
      bool digital_sum_only_;
      bool sum_sums_;
      art::RunNumber_t current_run_;

    int fragments_per_board_;
    int fragment_receiver_count_;

    std::vector<std::unique_ptr<TGraph>> graphs_;
    std::vector<std::unique_ptr<TH1D>> histograms_;

    // John F., 1/4/14 -- this should be improved; user needs to
    // hardwire the highest ADC value in the constructor (for example,
    // for a V1720 fragment, this would be 4096)

    int max_adc_count_;

  };
}

demo::WFViewer::WFViewer (fhicl::ParameterSet const & ps): art::EDAnalyzer(ps), board_type_(ps.get<std::string> ("board_type", "V1720")), sum_graphs_(2), prescale_(ps.get<int> ("prescale")), digital_sum_only_(ps.get<bool> ("digital_sum_only", false)), sum_sums_ (ps.get<bool> ("sum_sums", false)), current_run_(0), fragments_per_board_(ps.get<int>("fragments_per_board")), fragment_receiver_count_(ps.get<int>("fragment_receiver_count")), graphs_(fragments_per_board_*fragment_receiver_count_), histograms_(fragments_per_board_*fragment_receiver_count_), max_adc_count_(4096) {
}

demo::WFViewer::~WFViewer() { }

void demo::WFViewer::analyze (art::Event const & e) {
  art::Handle<artdaq::Fragments> v172x;
  e.getByLabel ("daq", board_type_, v172x);
  if (!v172x.isValid ())  {
    e.getByLabel ("unhuff" + board_type_, board_type_, v172x);
    if (!v172x.isValid ()) return;
  }

  size_t record_size = 100000000;
  for (size_t i = 0; i < v172x->size(); ++i) {
    record_size = std::min (record_size, V172xFragment((*v172x)[i]).total_adc_values());
  }

  for (size_t i = 0; i < v172x->size(); ++i) {
    const auto& frag((*v172x)[i]);

    if (frag.sequenceID () % prescale_ ) {
      return;
    }

    V172xFragment b(frag);
    size_t board_id = b.board_id ();

    if (x_.size () != record_size) {
      x_.resize (record_size);

      if (frag.hasMetadata()) {
        int post_trigger = frag.metadata<V172xFragment::metadata>()->post_trigger;
        int pre_samples = record_size * post_trigger / 100 - record_size;

	cout << "Fragment has metadata: post_trigger = " << post_trigger << ", pre_samples = " << pre_samples << endl;
	std::iota (x_.begin (), x_.end (), pre_samples);
      } else std::iota (x_.begin (), x_.end (), 0);
    }

    for (int ch=0; ch<fragments_per_board_; ++ch) {
      int lg = board_id * fragments_per_board_ + ch;                        

      int total_frags = fragments_per_board_*fragment_receiver_count_;

      if (!b.channel_present (ch) || lg >= total_frags) continue;

      if (!digital_sum_only_) {

	if (!graphs_[lg] || graphs_[lg]->GetN () != int(record_size)) {
	  graphs_[lg] = std::unique_ptr<TGraph>(new TGraph (record_size));
	  graphs_[lg]->SetTitle (Form ("Board %d, Channel %d", static_cast<int>(board_id), ch));
	  graphs_[lg]->SetLineColor (normals_[lg] ? 4 : 1);
	  std::copy (x_.begin (), x_.end (), graphs_[lg]->GetX ());
	  std::fill (graphs_[lg]->GetY (), graphs_[lg]->GetY () + record_size, 0);
	}
	std::copy (b.chDataBegin (ch), b.chDataBegin(ch) + record_size, graphs_[lg]->GetY ());

	int lg_canvas = lg % total_frags + 1;
	int padnum = fragments_per_board_ * ((lg_canvas - 1) % fragment_receiver_count_) + int((fragments_per_board_ * lg_canvas - 1) / total_frags) + 1;

	canvas_[0] -> cd(padnum);
	TVirtualPad* pad = static_cast<TVirtualPad*>(canvas_[0]->GetPad(padnum) ); 

	Double_t lo_x = graphs_[lg]->GetXaxis()->GetXmin();
	Double_t hi_x = graphs_[lg]->GetXaxis()->GetXmax();
	Double_t lo_y = -0.5;
	Double_t hi_y = max_adc_count_-0.5;


	TH1F* padframe = static_cast<TH1F*>( pad->DrawFrame( lo_x, lo_y, hi_x, hi_y ) );
	pad->SetGrid();
	padframe->Draw("SAME");

	graphs_[lg]->GetYaxis()->SetRangeUser (*std::min_element (graphs_[lg]->GetY (), graphs_[lg]->GetY () + record_size), *std::max_element (graphs_[lg]->GetY (), graphs_[lg]->GetY () + record_size));
	gStyle->SetMarkerStyle(22);
	gStyle->SetMarkerColor(4);
	graphs_[lg]->Draw ("PSAME");

	//	std::vector<Double_t> vals(10) ; 
	//	std::copy (b.chDataBegin (ch), b.chDataBegin(ch) + record_size, vals.begin());

	//	cout << "Board " << board_id << ", channel " << ch << " , lg = " << lg << " : ";
	//	for (auto val : vals ) {
	//	  cout << val << " ";
	//	}
	//	cout << endl;

	canvas_[1] -> cd( padnum );
	if (!histograms_[lg]) {

	  histograms_[lg] = std::unique_ptr<TH1D>(new TH1D( Form ("Board_%d_Channel_%d", static_cast<int>(board_id), ch), "", max_adc_count_, -0.5, max_adc_count_ - 0.5));
	  histograms_[lg]->SetTitle (Form ("Board %d, Channel %d", static_cast<int>(board_id), ch));
	  histograms_[lg]->SetLineColor (normals_[lg] ? 4 : 1);
	}

	// Surely there's a slicker way to extract single ADC values from a fragment...

	std::vector<demo::V172xFragment::adc_type> vals(record_size);
	std::copy (b.chDataBegin (ch), b.chDataBegin(ch) + record_size, vals.begin());

	for (auto val : vals ) {
	  histograms_[lg]->Fill(val);
	}

	histograms_[lg]->Draw();

      }
    }
  }

  canvas_[0] -> Modified();
  canvas_[0] -> Update();

  canvas_[1] -> Modified();
  canvas_[1] -> Update();


  // for (int i = 0; i < 2; i++) {
  //   //    if (!y_sum[i].size ()) continue;
  //   canvas_[2] -> cd(i + 1);
  //   if (!sum_graphs_[i] || sum_graphs_[i]->GetN () != int(record_size)) {
  //     sum_graphs_[i] = std::unique_ptr<TGraph>(new TGraph (record_size));
  //     sum_graphs_[i] -> SetTitle("Sum");
  //     //sum_graphs_[i]->SetLineColor(2);
  //     std::copy (x_.begin (), x_.end (), sum_graphs_[i]->GetX ());
  //     std::fill (sum_graphs_[i]->GetY (), sum_graphs_[i]->GetY () + record_size, 0);
  //   } 
  //   //    if (sum_sums_) std::transform (y_sum[i].begin (), y_sum[i].end (), sum_graphs_[i]->GetY (), sum_graphs_[i]->GetY (), std::plus<Double_t>());
  //   //    else std::copy (y_sum[i].begin (), y_sum[i].end (), sum_graphs_[i]->GetY ());
  //   //    sum_graphs_[i]->GetYaxis()->SetRangeUser (*std::min_element (sum_graphs_[i]->GetY (), sum_graphs_[i]->GetY () + y_sum[i].size ()), *std::max_element (sum_graphs_[i]->GetY (), sum_graphs_[i]->GetY () + y_sum[i].size ()));
  //   //    sum_graphs_[i]->Draw ("AL+");
  //   if(!digital_sum_only_) {	
  //     canvas_[i] -> Modified();
  //     canvas_[i] -> Update();
  //   }
  // }
  // canvas_[2] -> Modified();
  // canvas_[2] -> Update();
}	

void demo::WFViewer::beginRun(art::Run const &e) { 
  if (e.run () == current_run_) return;
  current_run_ = e.run ();

  for (int i = 0; i < 2; i++) canvas_[i] = 0;
  for (auto &x: graphs_) x = 0;
  for (auto &x: sum_graphs_) x = 0;
  for (auto &x: histograms_) x = 0;

  for (int i = 0; i < 2 && !digital_sum_only_; i++) {
    canvas_[i] = std::unique_ptr<TCanvas>(new TCanvas(Form("wf%d",i)));
    canvas_[i]->Divide (fragments_per_board_, fragment_receiver_count_);
    canvas_[i]->Update ();
    ((TRootCanvas*)canvas_[i]->GetCanvasImp ())->DontCallClose ();
  }

  // canvas_[2] = std::unique_ptr<TCanvas>(new TCanvas("digital sum"));
  // canvas_[2] -> Divide(1,2);
  // canvas_[2] -> Update();
  // ((TRootCanvas*)canvas_[2]->GetCanvasImp ())->DontCallClose ();

  normals_.resize (40, true);
  normals_[38] = normals_[39] = false;
}


DEFINE_ART_MODULE(demo::WFViewer)
