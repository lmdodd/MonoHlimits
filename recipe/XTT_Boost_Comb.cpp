#include <string>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include "boost/algorithm/string/predicate.hpp"
#include "boost/program_options.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/regex.hpp"
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/CardWriter.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"

using namespace std;
using boost::starts_with;
namespace po = boost::program_options;
int main(int argc, char** argv) {
  //! [part1]
  // First define the location of the "auxiliaries" directory where we can
  // source the input files containing the datacard shapes
  string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/UWAnalysis/StatTools/data/monohiggsBoost/datacards/";
  string input_dir =
      string(getenv("CMSSW_BASE")) + "/src/CombineHarvester/CombineTools/input";

  typedef vector<string> VString;
  typedef vector<pair<int, string>> Categories;

  string mass="";
  string signalMass="";
  string model="Zprime";

  VString masses;
  VString sig_procs;


  po::variables_map vm;
  po::options_description config("configuration");
  config.add_options()
      ("mass,m", po::value<string>(&mass)->default_value(mass))
      ("signalMass", po::value<string>(&signalMass)->default_value(signalMass))
      ("model", po::value<string>(&model)->default_value(model));
  po::store(po::command_line_parser(argc, argv).options(config).run(), vm);
  po::notify(vm);


  // Create an empty CombineHarvester instance that will hold all of the
  // datacard configuration and histograms etc.
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  // cb.SetVerbosity(3);
  VString chns =
  {"mt"};
  //{"mt","tt"};


  // Here we will just define two categories for an 13TeV analysis. Each entry in
  // the vector below specifies a bin name and corresponding bin_id.

  //ch::Categories cats = {
  //    {1, "_inclusive"}
  //};
  // ch::Categories is just a typedef of vector<pair<int, string>>
  //! [part1]
  map<string, VString> bkg_procs;
  bkg_procs["mt"] = {"ZTT", "W", "QCD", "TT", "VV","ZVV","SMH"};

  map<string, Categories> cats;
  cats["mt"] = {
      {1, "_inclusive"}};
  //! [part1]
  // Get the table of H->tau tau BRs vs mass
  //! 
  //Option 1
  //vector<string> massesA = ch::MassesFromRange("400-800:100");

  masses = {mass}; 
  if (model=="Zprime")
      sig_procs = {"Zprime"+signalMass+"A"};
  if (model=="Baryonic")
      sig_procs = {"ZpBaryonic_Zp"+signalMass+"_MChi"};

  //! [part2]
  for (auto chn : chns) {
      cb.AddObservations(
              {"*"}, {"xtt"}, {"13TeV"}, {chn}, cats[chn]);
      cb.AddProcesses(
              {"*"}, {"xtt"}, {"13TeV"}, {chn}, bkg_procs[chn], cats[chn], false);
      cb.AddProcesses(
              masses, {"xtt"}, {"13TeV"}, {chn}, sig_procs, cats[chn], true);
  }

  //! [part4]

  //! [part4]


  //Some of the code for this is in a nested namespace, so
  // we'll make some using declarations first to simplify things a bit.
  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::channel;
  using ch::syst::bin_id;
  using ch::syst::process;


  //! [part6]

  cb.cp().process(ch::JoinStr({sig_procs,{"ZTT", "W", "TT", "VV","ZVV","SMH"} }))
      .AddSyst(cb, "CMS_lumi", "lnN", SystMap<>::init(1.062));

  //need to edit for ditau
  cb.cp().process(ch::JoinStr({sig_procs, {"ZTT", "TT","VV","SMH"}}))
      .AddSyst(cb, "CMS_eff_t", "lnN", SystMap<>::init(1.08));

  cb.cp().process({"ZTT"})
      .AddSyst(cb, "CMS_xtt_zjXsec_13TeV", "lnN", SystMap<>::init(1.1));

  cb.cp().process({"SMH"})
      .AddSyst(cb, "CMS_xtt_smh_b", "lnN", SystMap<>::init(1.12));


  cb.cp().process({"TT"})
      .AddSyst(cb, "CMS_norm_TT_btag", "lnN", SystMap<>::init(1.12));

  cb.cp().process({"QCD"})
      .AddSyst(cb, "CMS_QCD_Syst", "lnN", SystMap<>::init(1.3));

  cb.cp().process({"TT"})
      .AddSyst(cb, "CMS_xtt_ttbarShape_$ERA", "shape", SystMap<>::init(1.00));

  cb.cp().process({"ZTT"})
      .AddSyst(cb, "CMS_xtt_dyShape_$ERA", "shape", SystMap<>::init(1.00));


  // Electron and muon efficiencies
  cb.cp().AddSyst(cb, "CMS_eff_m", "lnN", SystMap<channel, process>::init
          ({"mt"}, ch::JoinStr({sig_procs, {"ZTT", "TT", "VV","W","SMH"}}),  1.03));

  // Diboson and ttbar Normalisation - fully correlated
  cb.cp().process({"VV"}).AddSyst(cb,
          "CMS_xtt_vvXsec_13TeV", "lnN", SystMap<>::init(1.05));

  cb.cp().process({"TT"}).AddSyst(cb,
          "CMS_xtt_tjXsec_13TeV", "lnN", SystMap<>::init(1.06));

  // W norm, just for em and tt where MC norm is from MC
  cb.cp().process({"W"})
      .AddSyst(cb, "CMS_norm_W ", "lnN", SystMap<>::init(1.2));

  //add btagging uncertainties
  //add lepton energy shifts
  //
  //TES uncorrelated for now.. potentially correlate
  
  cb.cp().channel({"mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TT", "VV", "W","SMH","ZVV"}}))
      .AddSyst(cb, "CMS_scale_m_mt_$ERA", "shape", SystMap<>::init(1.00));
  

  for (string chn : chns) {
      string file = aux_shapes + "xtt_boost_" + chn +
          ".inputs-13TeV-mt12.root";
      //".inputs-13TeV-met.root";
      cb.cp().channel({chn}).backgrounds().ExtractShapes(
              file, "$BIN/$PROCESS", "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().channel({chn}).signals().ExtractShapes(
              file, "$BIN/$PROCESS$MASS", "$BIN/$PROCESS$MASS_$SYSTEMATIC");
  }
  cout << ">> Scaling signal process rates...\n";
  map<string, TGraph> xs;

  //! [part8]
  auto bbb = ch::BinByBinFactory()
      .SetAddThreshold(0.1)
      .SetMergeThreshold(0.5)
      .SetFixNorm(true);
  bbb.MergeBinErrors(cb.cp().backgrounds());
  bbb.AddBinByBin(cb.cp().backgrounds(), cb);

  // This function modifies every entry to have a standardised bin name of
  // the form: {analysis}_{channel}_{bin_id}_{era}
  // which is commonly used in the xtt analyses
  ch::SetStandardBinNames(cb);
  //! [part8]

  //! [part9]
  // First we generate a set of bin names:
  set<string> bins = cb.bin_set();
  // This method will produce a set of unique bin names by considering all
  // Observation, Process and Systematic entries in the CombineHarvester
  // instance.

  // We create the output root file that will contain all the shapes.
  // Here we define a CardWriter with a template for how the text datacard
  // and the root files should be named.
  ch::CardWriter writer("$TAG/$MASS/$ANALYSIS_$CHANNEL_$BINID_$ERA.txt",
          "$TAG/$ANALYSIS_$CHANNEL.input_$ERA.root");
  // writer.SetVerbosity(1);
  writer.WriteCards("output/xtt_cards_boost/"+model+signalMass+"A"+mass+"/cmb", cb);
  //for (auto chn : cb.channel_set()) {
  //    writer.WriteCards("output/xtt_cards/" + chn, cb.cp().channel({chn}));
  //}


}
