
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
    virtual ~WFViewer () = default;

    void analyze (art::Event const & e) override;
    void beginRun(art::Run const &) override;

  private:
    std::string board_type_;
    std::unique_ptr<TCanvas> canvas_[2];
    std::vector<Double_t> x_;
    int prescale_;
    bool digital_sum_only_;
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

demo::WFViewer::WFViewer (fhicl::ParameterSet const & ps): 
  art::EDAnalyzer(ps), 
  board_type_(ps.get<std::string> ("board_type", "V1720")), 
  prescale_(ps.get<int> ("prescale")), 
  digital_sum_only_(ps.get<bool> ("digital_sum_only", false)), 
  current_run_(0), 
  fragments_per_board_(ps.get<int>("fragments_per_board")), 
  fragment_receiver_count_(ps.get<int>("fragment_receiver_count")), 
  graphs_(fragments_per_board_*fragment_receiver_count_), 
  histograms_(fragments_per_board_*fragment_receiver_count_), 
  max_adc_count_(4096) {
  gStyle->SetOptStat("irm");
}


void demo::WFViewer::analyze (art::Event const & e) {

  static int evt_cntr = -1;
  evt_cntr++;

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

  int total_frags = fragments_per_board_*fragment_receiver_count_;

  if (total_frags != static_cast<int>(v172x->size())) {
    cout << "Warning in WFViewer: mismatch between expected and actual fragments: fragments_per_board_ = " << fragments_per_board_ << ", fragment_receiver_count_ = " << fragment_receiver_count_ << ", number of fragments = " << v172x->size() << endl;
    //    throw;
  }

  // John F., 1/5/14 

  // Here, we loop over the fragments passed to the analyze
  // function. A warning is flashed if either (A) the fragments aren't
  // all from the same event, or (B) there's an unexpected number of
  // fragments given the number of boardreaders and the number of
  // fragments per board

  // For every Nth event, where N is the "prescale" setting, plot the
  // distribution of ADC counts from each board_id / fragment_id
  // combo. Also, if "digital_sum_only" is set to false in the FHiCL
  // string, then plot, for the Nth event, a graph of the ADC values
  // across all channels in each board_id / fragment_id combo

  int expected_sequence_id = -1;
  bool prescaled = false;

  // Use to check that fragment ID counting starts at 0
  std::vector<int> fragment_ids( v172x->size() ); 

  for (size_t i = 0; i < v172x->size(); ++i) {

    const auto& frag((*v172x)[i]);

    if (i == 0) 
      expected_sequence_id = frag.sequenceID();

    if (expected_sequence_id != static_cast<int>(frag.sequenceID())) {
      cout << "Warning in WFViewer: expected fragment with sequence ID " << expected_sequence_id << ", received one with sequence ID " << frag.sequenceID() << endl;
    }

    V172xFragment b(frag);
    int board_id = static_cast<int>( b.board_id () );
    int fragment_id = static_cast<int>(frag.fragmentID() );

    fragment_ids.emplace_back (fragment_id );

    int lg = fragment_id; // assuming fragment counting begins at 0            
    int lg_canvas = lg % total_frags + 1;
    int padnum = lg_canvas;

    // If a histogram doesn't exist for this board_id / fragment_id combo, create it

    if (!histograms_[lg]) {

      histograms_[lg] = std::unique_ptr<TH1D>(new TH1D( Form ("Board_%d_Fragment_%d_hist", board_id, fragment_id), "", max_adc_count_, -0.5, max_adc_count_ - 0.5));
      histograms_[lg]->SetTitle (Form ("Board %d, Fragment %d", board_id, fragment_id));
      histograms_[lg]->GetXaxis()->SetTitle("ADC value");
    }

    // For every event, fill the histogram (prescale is ignored here)

    for (auto val = b.dataBegin(); val != b.dataEnd(); ++val ) {
      histograms_[lg]->Fill( *val );
    }
    
    //    if (frag.sequenceID () % prescale_ - 1) {
    if (evt_cntr % prescale_ - 1) {
      prescaled = true;
      continue;
    } else {
      prescaled = false;
    }

    // If we pass the prescale, then if we're not going with
    // digital_sum_only, plot the ADC counts for this particular event/board/fragment_id
    
    if (!digital_sum_only_) {

      // Create the graph's x-axis

      if (x_.size () != record_size) {
	x_.resize (record_size);

	if (frag.hasMetadata()) {
	  cout << "Warning in WFViewer: displaying fragments with metadata has not yet been tested" << endl;
	  int post_trigger = frag.metadata<V172xFragment::metadata>()->post_trigger;
	  int pre_samples = record_size * post_trigger / 100 - record_size;
	  std::iota (x_.begin (), x_.end (), pre_samples);
	  
	} else 
	  std::iota (x_.begin (), x_.end (), 0);
      }

      // If the graph doesn't exist, create it. Not sure whether to
      // make it an error if the record_size is new

      if (!graphs_[lg] || graphs_[lg]->GetN () != int(record_size)) {
	graphs_[lg] = std::unique_ptr<TGraph>(new TGraph (record_size));
	graphs_[lg]->SetName( Form ("Board_%d_Fragment_%d_graph", board_id, fragment_id));
	graphs_[lg]->SetTitle (Form ("Board %d, Fragment %d", board_id, fragment_id));
	graphs_[lg]->SetLineColor ( 4 );
	std::copy (x_.begin (), x_.end (), graphs_[lg]->GetX ());
      }

      // Get the data from the fragment

      std::copy (b.dataBegin (), b.dataBegin() + record_size, graphs_[lg]->GetY ());

      // And now prepare the graphics without actually drawing anything yet
      
      canvas_[1] -> cd(padnum);
      TVirtualPad* pad = static_cast<TVirtualPad*>(canvas_[1]->GetPad(padnum) ); 

      Double_t lo_x = graphs_[lg]->GetXaxis()->GetXmin();
      Double_t hi_x = graphs_[lg]->GetXaxis()->GetXmax();
      Double_t lo_y = -0.5;
      Double_t hi_y = max_adc_count_-0.5;


      TH1F* padframe = static_cast<TH1F*>( pad->DrawFrame( lo_x, lo_y, hi_x, hi_y ) );
      padframe->SetTitle( Form ("Board %d, Frag %d, SeqID %d", board_id, fragment_id, expected_sequence_id));
      padframe->GetXaxis()->SetTitle("Channel #");
      pad->SetGrid();
      padframe->Draw("SAME");

      graphs_[lg]->GetYaxis()->SetRangeUser (*std::min_element (graphs_[lg]->GetY (), graphs_[lg]->GetY () + record_size), *std::max_element (graphs_[lg]->GetY (), graphs_[lg]->GetY () + record_size));
      gStyle->SetMarkerStyle(22);
      gStyle->SetMarkerColor(4);
    }
  } // End loop over fragments

  if (*std::min_element( fragment_ids.begin(), fragment_ids.end() ) != 0 ) {
    cout << "Warning in WFViewer: expected lowest fragment_id val to be 0, instead got " << *std::min_element( fragment_ids.begin(), fragment_ids.end() ) << endl;
  }

  // Don't draw anything if we're prescaled

  if (prescaled) return;

  // Draw the histograms

  for (int lg = 0; lg < total_frags; ++lg) {
    canvas_[0]->cd(lg+1);
    histograms_[lg]->Draw();
  }

  canvas_[0] -> Modified();
  canvas_[0] -> Update();

  // And, if desired, the Nth event's ADC counts
  
  if (!digital_sum_only_) {
    for (int lg = 0; lg < total_frags; ++lg) {
      canvas_[1]->cd(lg+1);
      graphs_[lg]->Draw("PSAME");
    }

    canvas_[1] -> Modified();
    canvas_[1] -> Update();
  }

}

void demo::WFViewer::beginRun(art::Run const &e) { 
  if (e.run () == current_run_) return;
  current_run_ = e.run ();

  for (int i = 0; i < 2; i++) canvas_[i] = 0;
  for (auto &x: graphs_) x = 0;
  for (auto &x: histograms_) x = 0;

  for (int i = 0; (i < 2 && !digital_sum_only_) || i < 1 ; i++) {
    canvas_[i] = std::unique_ptr<TCanvas>(new TCanvas(Form("wf%d",i)));
    canvas_[i]->Divide (fragments_per_board_, fragment_receiver_count_);
    canvas_[i]->Update ();
    ((TRootCanvas*)canvas_[i]->GetCanvasImp ())->DontCallClose ();
  }

  canvas_[0]->SetTitle("ADC Value Distribution");
  canvas_[1]->SetTitle("ADC Counts by Channel");
}


DEFINE_ART_MODULE(demo::WFViewer)
