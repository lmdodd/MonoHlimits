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
  string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/Analysis/MonoHlimits/datacards/";
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
  {"mt","et","tt"};
  //{"tt"};


  // Each entry in the vector below specifies a bin name and corresponding bin_id.

  //ch::Categories cats = {
  //    {1, "_inclusive"}
  //};
  // ch::Categories is just a typedef of vector<pair<int, string>>
  //! [part1]
  map<string, VString> bkg_procs;
  bkg_procs["et"] = {"ZTT", "W", "QCD", "ZL", "ZJ", "TTT","TTJ", "VVT","VVJ","ZVV","SMH","EWK"};
  bkg_procs["mt"] = {"ZTT", "W", "QCD", "ZL", "ZJ", "TTT","TTJ", "VVT","VVJ","ZVV","SMH","EWK"};
  bkg_procs["tt"] = {"ZTT", "W", "QCD", "ZL", "ZJ", "TTT","TTJ", "VVT","VVJ","ZVV","SMH","EWK"};

  map<string, Categories> cats;
  cats["et"] = {
      {1, "et_inclusive"}};
  cats["mt"] = {
      {1, "mt_inclusive"}};
  cats["tt"] = {
      {1, "tt_inclusive"}};
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


  //Some of the code for this is in a nested namespace, so
  // we'll make some using declarations first to simplify things a bit.
  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::channel;
  using ch::syst::bin_id;
  using ch::syst::process;


  //! [part6]

  cb.cp().process(sig_procs)
      .AddSyst(cb, "CMS_remormalization", "lnN", SystMap<>::init(1.04));

  cb.cp().process(sig_procs)
      .AddSyst(cb, "CMS_PDF", "lnN", SystMap<>::init(1.02)); //THIS IS GUESS! 

  cb.cp().process(ch::JoinStr({sig_procs,{"ZTT", "W", "ZL", "ZJ", "TTT","TTJ", "VVJ","VVT","ZVV","SMH","EWK"} }))
      .AddSyst(cb, "CMS_lumi", "lnN", SystMap<>::init(1.025));

  //TES uncorrelated for now.. potentially correlate
  cb.cp().channel({"tt"}).process(ch::JoinStr({sig_procs, {"ZTT","TTT","VVT","SMH"}}))
      .AddSyst(cb, "CMS_scale_t_$ERA", "shape", SystMap<>::init(1.00));
  cb.cp().channel({"mt"}).process(ch::JoinStr({sig_procs, {"ZTT","TTT","VVT","SMH"}}))
      .AddSyst(cb, "CMS_scale_t_$ERA", "shape", SystMap<>::init(1.00));
  cb.cp().channel({"et"}).process(ch::JoinStr({sig_procs, {"ZTT","TTT","VVT","SMH"}}))
      .AddSyst(cb, "CMS_scale_t_$ERA", "shape", SystMap<>::init(1.00));

  //diTau trigger turn on efficiency
  //cb.cp().channel({"tt"}).process(ch::JoinStr({sig_procs, {"ZTT","TTT","VVT","SMH"}}))
  cb.cp().channel({"tt"}).process(ch::JoinStr({{"ZTT","TTT"}}))
      .AddSyst(cb, "CMS_xtt_tt_trigger_$ERA", "shape", SystMap<>::init(0.4));

  cb.cp().channel({"tt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT", "VVT", "SMH"}}))
            .AddSyst(cb, "CMS_eff_t_tt_$ERA", "lnN", SystMap<>::init(1.09));

  cb.cp().channel({"tt"}).process(ch::JoinStr({ {"TTJ", "VVJ", "ZL", "ZJ","W","ZVV","EWK"}}))
            .AddSyst(cb, "CMS_fake_eff_t_tt_$ERA", "lnN", SystMap<>::init(1.025));



  cb.cp().channel({"tt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH","ZVV","EWK"}}))
      .AddSyst(cb, "CMS_scale_m_$ERA", "shape", SystMap<>::init(1.00));
  cb.cp().channel({"mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH","ZVV","EWK"}}))
      .AddSyst(cb, "CMS_scale_m_$ERA", "shape", SystMap<>::init(1.00));
  cb.cp().channel({"et"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH","ZVV","EWK"}}))
      .AddSyst(cb, "CMS_scale_m_$ERA", "shape", SystMap<>::init(1.00));

  cb.cp().channel({"tt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH","ZVV","EWK"}}))
      .AddSyst(cb, "CMS_scale_j_$ERA", "shape", SystMap<>::init(1.00));
  cb.cp().channel({"mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH","ZVV","EWK"}}))
      .AddSyst(cb, "CMS_scale_j_$ERA", "shape", SystMap<>::init(1.00));
  cb.cp().channel({"et"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH","ZVV","EWK"}}))
      .AddSyst(cb, "CMS_scale_j_$ERA", "shape", SystMap<>::init(1.00));



  // Electron and muon efficiencies
  cb.cp().channel({"mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH","ZVV","EWK"}}))
            .AddSyst(cb, "CMS_eff_m_$ERA", "lnN", SystMap<>::init(1.02));
  cb.cp().channel({"et"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH","ZVV","EWK"}}))
            .AddSyst(cb, "CMS_eff_e_$ERA", "lnN", SystMap<>::init(1.02));

  // mt 
  cb.cp().channel({"et","mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT", "VVT", "SMH"}}))
            .AddSyst(cb, "CMS_eff_t_$ERA", "lnN", SystMap<>::init(1.045));
  cb.cp().channel({"et","mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT", "VVT", "SMH"}}))
            .AddSyst(cb, "CMS_eff_t_$CHANNEL_$ERA", "lnN", SystMap<>::init(1.02));

  cb.cp().channel({"et","mt"}).process(ch::JoinStr({ {"TTJ", "VVJ", "ZL", "ZJ","W","ZVV","EWK"}}))
            .AddSyst(cb, "CMS_fake_eff_t_$ERA", "lnN", SystMap<>::init(1.045));
  cb.cp().channel({"et","mt"}).process(ch::JoinStr({ {"TTJ", "VVJ", "ZL", "ZJ","W","ZVV","EWK"}}))
            .AddSyst(cb, "CMS_fake_eff_t_$CHANNEL_$ERA", "lnN", SystMap<>::init(1.02));


//Drell Yan  uncertainties
  cb.cp().process({"ZTT", "ZL", "ZJ"}).AddSyst(cb,
          "CMS_xtt_zjXsec_13TeV", "lnN", SystMap<>::init(1.03));
 

  //Drell Yan  uncertainties
  cb.cp().process({"ZTT", "ZL", "ZJ"}).AddSyst(cb,
          "CMS_xtt_zjXsec_13TeV", "lnN", SystMap<>::init(1.03));
  cb.cp().process({"ZTT","ZL","ZJ"})
      .AddSyst(cb, "CMS_xtt_dyShape_$ERA", "shape", SystMap<>::init(1.00));
  cb.cp().process({"ZL"}).channel({"et"}).AddSyst(cb,
          "CMS_xtt_eFakeTau_13TeV", "lnN", SystMap<>::init(1.12));
  cb.cp().process({"ZL"}).channel({"mt"}).AddSyst(cb,
          "CMS_xtt_mFakeTau_13TeV", "lnN", SystMap<>::init(1.25));
  // mu to tau FR
  cb.cp().process( {"ZL"}).channel({"mt"}).AddSyst(cb,
          "CMS_xtt_ZLScale_mutau_$ERA", "shape", SystMap<>::init(1.00));
  // e to tau FR
  cb.cp().process( {"ZL"}).channel({"et"}).AddSyst(cb,
          "CMS_xtt_ZLScale_etau_$ERA", "shape", SystMap<>::init(1.00));

 
  //Fake Tau Uncertainties
  cb.cp().process( {"TTJ","W","ZJ"}).channel({"tt","mt","et"}).AddSyst(cb,
          "CMS_xtt_jetToTauFake_$ERA", "shape", SystMap<>::init(1.00));

  //Fake Tau Uncertainties
  cb.cp().process( {"ZTT","VVT","SMH","TTT"}).channel({"tt","mt","et"}).AddSyst(cb,
          "CMS_xtt_highTauEffi_$ERA", "shape", SystMap<>::init(1.00));


  // W norm, just for em and tt where MC norm is from MC
  cb.cp().process({"W"})
      .AddSyst(cb, "CMS_xtt_wShape_$ERA", "shape", SystMap<>::init(1.00));
  cb.cp().process({"W"}).channel({"tt"})
      .AddSyst(cb, "CMS_norm_W", "lnN", SystMap<>::init(1.02));
  cb.cp().process({"W"}).channel({"et"})
      .AddSyst(cb, "CMS_norm_W", "lnN", SystMap<>::init(1.22));
  cb.cp().process({"W"}).channel({"mt"})
      .AddSyst(cb, "CMS_norm_W", "lnN", SystMap<>::init(1.15));

      cb.cp().process({"TTT","TTJ"})
      .AddSyst(cb, "CMS_xtt_ttbarShape_$ERA", "shape", SystMap<>::init(1.00));

  //Top pt uncertainties 
  cb.cp().process({"TTT","TTJ","VVJ","VVL"})
      .AddSyst(cb, "CMS_norm_btag", "lnN", SystMap<>::init(1.04));
  cb.cp().process({"VVJ","VVL"})
      .AddSyst(cb, "CMS_norm_btag", "lnN", SystMap<>::init(1.02));
  cb.cp().process(ch::JoinStr({sig_procs, {"QCD", "ZL", "ZJ","ZVV","EWK","W"}}))
      .AddSyst(cb, "CMS_norm_mistag", "lnN", SystMap<>::init(1.02));
  cb.cp().process({"SMH", "ZL", "ZJ","ZVV","EWK"})
      .AddSyst(cb, "CMS_norm_mistag", "lnN", SystMap<>::init(1.05));

  // TTBAR   - fully correlated
  cb.cp().process({"TTT","TTJ"}).AddSyst(cb,
          "CMS_xtt_tjXsec_13TeV", "lnN", SystMap<>::init(1.06));

  //QCD uncertainties
  cb.cp().process({"QCD"}).channel({"mt"})
      .AddSyst(cb, "CMS_QCD_Syst ", "lnN", SystMap<>::init(1.62));
  cb.cp().process({"QCD"}).channel({"et","tt"})
      .AddSyst(cb, "CMS_QCD_Syst ", "lnN", SystMap<>::init(1.15));



  // Diboson - fully correlated
  cb.cp().process({"VVT","VVJ"}).AddSyst(cb,
          "CMS_xtt_vvXsec_13TeV", "lnN", SystMap<>::init(1.05));



  for (string chn : chns) {
      string file = aux_shapes + "xtt_" + chn +
          ".inputs-13TeV-mt_phil.root";
      //".inputs-13TeV-met.root";
      cb.cp().channel({chn}).backgrounds().ExtractShapes(
              file, "$BIN/$PROCESS", "$BIN/$PROCESS_$SYSTEMATIC");
      cb.cp().channel({chn}).signals().ExtractShapes(
              file, "$BIN/$PROCESS$MASS", "$BIN/$PROCESS$MASS_$SYSTEMATIC");
  }
  cout << ">> Scaling signal process rates...\n";
  map<string, TGraph> xs;

  for (string const& p : sig_procs) {
      // Get the table of xsecs vs mass for process "p" and era "e":
      cout << ">>>> Scaling for process " << p << "and signalMass " << signalMass <<" \n";
      cb.cp().process({p}).ForEachProc([&](ch::Process *proc) {
              std::string mass = proc->mass(); 
              int n = std::stoi(signalMass);
              int m = std::stoi(mass);
              cout << ">>>> Scaling for process " << p << "with mass "<<mass<<" \n";
              xs["xtt"+mass] = ch::TGraphFromTable(input_dir+"/xsecs_brs/"+model+"/xtt_monoH_"+signalMass+".txt", "mA", "br");
              proc->set_rate(proc->rate() * xs["xtt"+mass].Eval(m));
              cout << ">>>> Scaling for model " << model << "with mass "<<mass<<" and Signal mass "<<n<< "\n";
              cout << ">>>> Scaling is "<< xs["xtt"+mass].Eval(m)<< "\n";
              //proc->set_rate(proc->rate() * xs["xtt"+mass].Eval(300));
              });
  }



  //! [part8]
  auto bbb = ch::BinByBinFactory()
      .SetAddThreshold(0.1) //0.03
      .SetMergeThreshold(0.5) //0.8
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
          "$TAG/$MASS/$ANALYSIS_$CHANNEL.input_$ERA.root");
  writer.SetVerbosity(1);
  //writer.WriteCards("output/xtt_cards/"+model+signalMass+"A"+mass+"/cmb", cb);
  writer.WriteCards("output/xtt_cards_other/"+model+signalMass+"A"+mass+"/cmb", cb);
  //for (auto chn : cb.channel_set()) {
  //    writer.WriteCards("output/xtt_cards/" + chn, cb.cp().channel({chn}));
  //}


}
