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
#include "CombineHarvester/CombineTools/interface/Algorithm.h"
#include "CombineHarvester/CombineTools/interface/CardWriter.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"
#include "CombineHarvester/CombineTools/interface/AutoRebin.h"
#include "TH2.h"
#include "TH1.h"
#include "TF1.h"

using namespace std;
using boost::starts_with;
namespace po = boost::program_options;

template <typename T>

void To1Bin(T* proc)
{
    std::cout<<"Making CR into 1 bin"<<std::endl;
    std::unique_ptr<TH1> originalHist = proc->ClonedScaledShape();
    TH1F *hist = new TH1F("hist","hist",1,0,1);
    double err = 0;
    double rate = originalHist->IntegralAndError(0, originalHist->GetNbinsX() + 1, err);
    hist->SetDirectory(0);
    hist->SetBinContent(1, rate);
    hist->SetBinError(1, err);
    proc->set_shape(*hist, true);  // True means adjust the process rate to the
    // integral of the hist
    std::cout<<"CR is now one bin!"<<std::endl;
    std::cout<<"...."<<std::endl;
}

    template <typename T>
void MergeBins(T* proc)
{
    std::cout<<"Merging CR bins into 1 bin"<<std::endl;
    std::unique_ptr<TH1> originalHist = proc->ClonedScaledShape();
    TH1F *hist = new TH1F("hist","hist",3,0,13);
    double err = 0;
    double rate = 0;
    hist->SetDirectory(0);

    rate = originalHist->IntegralAndError(1, 6 , err);
    hist->SetBinContent(1, rate);
    hist->SetBinError(1, err);

    rate = originalHist->IntegralAndError(7, 12 , err);
    hist->SetBinContent(2, rate);
    hist->SetBinError(2, err);

    rate = originalHist->IntegralAndError(13, 14 , err);
    hist->SetBinContent(3, rate);
    hist->SetBinError(3, err);

    proc->set_shape(*hist, true);  // True means adjust the process rate to the
    // integral of the hist
    std::cout<<" CR Region reduced"<<std::endl;
    std::cout<<"...."<<std::endl;
}


bool BinIsControlRegion(ch::Object const* obj)
{
    return (boost::regex_search(obj->bin(),boost::regex{"_cr$"}) || (obj->channel() == std::string("mm")));
}

bool BinIsQCDControlRegion(ch::Object const* obj)
{
    return (boost::regex_search(obj->bin(),boost::regex{"_QCD_"}) && boost::regex_search(obj->bin(),boost::regex{"_cr$"}));
}

bool BinIsWControlRegion(ch::Object const* obj)
{
    return (boost::regex_search(obj->bin(),boost::regex{"_W_"}) && boost::regex_search(obj->bin(),boost::regex{"_cr$"}));
}

// Useful to have the inverse sometimes too
bool BinIsNotControlRegion(ch::Object const* obj)
{
    return !BinIsControlRegion(obj);
}

bool BinIsNotWControlRegion(ch::Object const* obj)
{
    return !BinIsWControlRegion(obj);
}




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
    int control_region = 1;
    int do1Bin = 0;


    VString masses;
    VString sig_procs;


    po::variables_map vm;
    po::options_description config("configuration");
    config.add_options()
        ("mass,m", po::value<string>(&mass)->default_value(mass))
        ("signalMass", po::value<string>(&signalMass)->default_value(signalMass))
        ("control_region", po::value<int>(&control_region)->default_value(1))
        ("do1Bin", po::value<int>(&do1Bin)->default_value(0))
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


    // Each entry in the vector below specifies a bin name and corresponding bin_id.

    //ch::Categories cats = {
    //    {1, "_inclusive"}
    //};
    // ch::Categories is just a typedef of vector<pair<int, string>>
    //! [part1]
    map<string, VString> bkg_procs;
    bkg_procs["et"] = {"ZTT", "W", "QCD", "TTT","TTJ", "VVT","VVJ","SMH","ZL","ZJ"};
    bkg_procs["mt"] = {"ZTT", "W", "QCD", "TTT","TTJ", "VVT","VVJ","SMH","ZL","ZJ"};
    bkg_procs["tt"] = {"ZTT", "W", "QCD", "TTT","TTJ", "VVT","VVJ","ZVV","SMH"};
    //bkg_procs["tt"] = {"ZTT", "W", "QCD", "TTT","TTJ", "VVT","VVJ","SMH"};

    map<string, Categories> cats;
    cats["et"] = {
        {1, "et_inclusive"}};
    cats["mt"] = {
        {1, "mt_inclusive"}};
    cats["tt"] = {
        {1, "tt_inclusive"}};


    if (control_region > 0){
        // for each channel use the categories >= 10 for the control regions
        for (auto chn : chns){
            // for em or tt or mm do nothing
            if (ch::contains({"mt", "et"}, chn)) {

                Categories queue;
                int binid = 10;

                queue.push_back(make_pair(binid,chn+"_W_inclusive_cr"));
                queue.push_back(make_pair(binid+1,chn+"_QCD_inclusive_cr"));

                cats[chn].insert(cats[chn].end(),queue.begin(),queue.end());
            }
            if (ch::contains({"tt"}, chn)) {

                Categories queue;
                int binid = 11;

                queue.push_back(make_pair(binid,chn+"_QCD_inclusive_cr"));

                cats[chn].insert(cats[chn].end(),queue.begin(),queue.end());
            }
        }
    } // end CR et mt > 0



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


    //Some of the code for this is in a nested namespace, so
    // we'll make some using declarations first to simplify things a bit.
    using ch::syst::SystMap;
    using ch::syst::era;
    using ch::syst::channel;
    using ch::syst::bin_id;
    using ch::syst::process;



    //! [part4]
    if ((control_region > 0) ){
        // Since we now account for QCD in the high mT region we only
        // need to filter signal processes
        cb.FilterAll([](ch::Object const* obj) {
                return (BinIsControlRegion(obj) && obj->signal());
                });
        //
    }

    //! [part6]

    cb.cp().process(sig_procs)
        .AddSyst(cb, "CMS_renormalization", "lnN", SystMap<>::init(1.04));

    cb.cp().process(sig_procs)
        .AddSyst(cb, "CMS_PDF", "lnN", SystMap<>::init(1.02)); 

    cb.cp().process(ch::JoinStr({sig_procs,{"ZTT", "ZL", "ZJ", "TTT","TTJ", "VVJ","VVT","ZVV","SMH"} }))
        .AddSyst(cb, "CMS_lumi", "lnN", SystMap<>::init(1.025));

    //TES uncorrelated for now.. potentially correlate
    cb.cp().channel({"tt"}).process(ch::JoinStr({sig_procs, {"ZTT","TTT","VVT","SMH"}}))
        .AddSyst(cb, "CMS_scale_t_$ERA", "shape", SystMap<>::init(2.00));
    cb.cp().channel({"mt"}).process(ch::JoinStr({sig_procs, {"ZTT","TTT","VVT","SMH"}}))
        .AddSyst(cb, "CMS_scale_t_$ERA", "shape", SystMap<>::init(2.00));
    cb.cp().channel({"et"}).process(ch::JoinStr({sig_procs, {"ZTT","TTT","VVT","SMH"}}))
        .AddSyst(cb, "CMS_scale_t_$ERA", "shape", SystMap<>::init(2.00));


    cb.cp().channel({"tt"}).process(ch::JoinStr({{"ZTT","TTT"}}))
        .AddSyst(cb, "CMS_xtt_tt_trigger_$ERA", "shape", SystMap<>::init(0.4));

    cb.cp().channel({"tt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH","ZVV"}}))
        .AddSyst(cb, "CMS_scale_m_$ERA", "shape", SystMap<>::init(1.00));
    cb.cp().channel({"mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH"}}))
        .AddSyst(cb, "CMS_scale_m_$ERA", "shape", SystMap<>::init(1.00));
    cb.cp().channel({"et"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH"}}))
        .AddSyst(cb, "CMS_scale_m_$ERA", "shape", SystMap<>::init(1.00));
    cb.cp().channel({"tt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH","ZVV"}}))
        .AddSyst(cb, "CMS_scale_j_$ERA", "shape", SystMap<>::init(1.00));
    cb.cp().channel({"mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH"}}))
        .AddSyst(cb, "CMS_scale_j_$ERA", "shape", SystMap<>::init(1.00));
    cb.cp().channel({"et"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH"}}))
        .AddSyst(cb, "CMS_scale_j_$ERA", "shape", SystMap<>::init(1.00));


    // Electron and muon efficiencies
    //cb.cp().channel({"mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH"}}))
    cb.cp().channel({"mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","SMH"}}))
        .AddSyst(cb, "CMS_eff_m_$ERA", "lnN", SystMap<>::init(1.02));
    //cb.cp().channel({"et"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","W","SMH"}}))
    cb.cp().channel({"et"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT","TTJ", "VVT", "VVJ", "ZL", "ZJ","SMH"}}))
        .AddSyst(cb, "CMS_eff_e_$ERA", "lnN", SystMap<>::init(1.02));

    // mt 
    cb.cp().channel({"et","mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT", "VVT", "SMH"}}))
        .AddSyst(cb, "CMS_eff_t_$ERA", "lnN", SystMap<>::init(1.045));
    cb.cp().channel({"et","mt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT", "VVT", "SMH"}}))
        .AddSyst(cb, "CMS_eff_t_$CHANNEL_$ERA", "lnN", SystMap<>::init(1.02));


    cb.cp().channel({"et","mt","tt"}).process(ch::JoinStr({ {"TTJ", "VVJ", "ZL", "ZJ"}}))
        .AddSyst(cb, "CMS_fake_eff_t_$ERA", "lnN", SystMap<>::init(1.045));
    cb.cp().channel({"et","mt","tt"}).process(ch::JoinStr({ {"TTJ", "VVJ", "ZL", "ZJ","ZVV"}}))
        .AddSyst(cb, "CMS_fake_eff_t_$CHANNEL_$ERA", "lnN", SystMap<>::init(1.02));

    cb.cp().channel({"tt"}).process(ch::JoinStr({sig_procs, {"ZTT", "TTT", "VVT", "SMH"}}))
        .AddSyst(cb, "CMS_eff_t_tt_$ERA", "lnN", SystMap<>::init(1.09));

    //Drell Yan  uncertainties
    cb.cp().process({"ZTT", "ZL", "ZJ"}).AddSyst(cb,
            "CMS_xtt_zjXsec_13TeV", "lnN", SystMap<>::init(1.03));
    cb.cp().process({"ZTT","ZL","ZJ"})
        .AddSyst(cb, "CMS_xtt_dyShape_$ERA", "shape", SystMap<>::init(0.1)); //10% was input
    
    cb.cp().process({"ZL"}).channel({"et"}).AddSyst(cb,
            "CMS_xtt_eFakeTau_13TeV", "lnN", SystMap<>::init(1.12));
    cb.cp().process({"ZL"}).channel({"mt"}).AddSyst(cb,
            "CMS_xtt_mFakeTau_13TeV", "lnN", SystMap<>::init(1.25));
    // mu to tau FR
    //STUPID UNCERTAINTY 
    //cb.cp().process( {"ZL"}).channel({"mt"}).AddSyst(cb,
    //        "CMS_xtt_ZLScale_mutau_$ERA", "shape", SystMap<>::init(1.00));
    //
    // e to tau FR
    //cb.cp().process( {"ZL"}).channel({"et"}).AddSyst(cb,
    //        "CMS_xtt_ZLScale_etau_$ERA", "shape", SystMap<>::init(1.00));

    //Fake Tau Uncertainties
    //cb.cp().process( {"VVJ","TTJ","ZJ"}).channel({"tt","mt","et"}).AddSyst(cb,
    cb.cp().process( {"VVJ","TTJ","W","ZJ"}).channel({"tt","mt","et"}).AddSyst(cb,
            "CMS_xtt_jetToTauFake_$ERA", "shape", SystMap<>::init(1.00));

    //Fake Tau Uncertainties
    cb.cp().process({"ZTT","VVT","SMH","TTT"}).channel({"tt","mt","et"}).AddSyst(cb,
            "CMS_xtt_highTauEffi_$ERA", "shape", SystMap<>::init(1.00));


    cb.cp().process({"W"})
        .AddSyst(cb, "CMS_xtt_wShape_$ERA", "shape", SystMap<>::init(1.00));
    // W norm, just for tt where MC norm is from MC
    cb.cp().process({"W"}).channel({"tt"})
        .AddSyst(cb, "CMS_norm_W_$CHANNEL", "lnN", SystMap<>::init(1.10));

    //cb.cp().process({"W"}).channel({"mt","et"})
    cb.cp().process({"W"}).channel({"mt","et"}).bin_id({1,11})
        .AddSyst(cb, "CMS_W_Extrap_$CHANNEL_$ERA", "lnN", SystMap<>::init(1.20));
    cb.cp().process({"TTT","TTJ"})
        .AddSyst(cb, "CMS_xtt_ttbarShape_$ERA", "shape", SystMap<>::init(1.00));

    cb.cp().process({"VVT","VVJ"})
        .AddSyst(cb, "CMS_xtt_ZZNLOewk_$ERA", "shape", SystMap<>::init(1.00));

    cb.cp().process({"VVT","VVJ"})
        .AddSyst(cb, "CMS_xtt_WWNLOewk_$ERA", "shape", SystMap<>::init(1.00));

    //Top pt uncertainties 
    cb.cp().process({"TTT","TTJ"})
        .AddSyst(cb, "CMS_norm_btag", "lnN", SystMap<>::init(1.04));
    cb.cp().process({"VVJ","VVL"})
        .AddSyst(cb, "CMS_norm_btag", "lnN", SystMap<>::init(1.02));
    cb.cp().process(sig_procs) //QCD and W were here
        .AddSyst(cb, "CMS_norm_mistag", "lnN", SystMap<>::init(1.02));
    cb.cp().process({"SMH","ZTT","ZL", "ZJ","ZVV"})
        .AddSyst(cb, "CMS_norm_mistag", "lnN", SystMap<>::init(1.05));

    // TTBAR   - fully correlated
    cb.cp().process({"TTT","TTJ"}).AddSyst(cb,
            "CMS_xtt_tjXsec_13TeV", "lnN", SystMap<>::init(1.06));

    //QCD uncertainties

    //cb.cp().process({"QCD"}).channel({"tt"})
    cb.cp().process({"QCD"}).channel({"tt"}).bin_id({1})
        .AddSyst(cb, "CMS_QCD_$CHANNEL_Syst_$ERA", "lnN", SystMap<>::init(1.20));
    //cb.cp().process({"QCD"}).channel({"et","mt"})
    cb.cp().process({"QCD"}).channel({"et","mt"}).bin_id({1,10})
        .AddSyst(cb, "CMS_QCD_$CHANNEL_Syst_$ERA", "lnN", SystMap<>::init(1.20));

    // Diboson - fully correlated
    cb.cp().process({"VVT","VVJ"}).AddSyst(cb,
            "CMS_xtt_vvXsec_13TeV", "lnN", SystMap<>::init(1.06));

    if (control_region == 1) {
        // Create rateParams for control regions:
        //  - [x] 1 rateParam for all W in every region
        //  - [x] 1 rateParam for QCD in low mT
        //  - [x] 1 rateParam for QCD in high mT
        //  - [x] lnNs for the QCD OS/SS ratio
        //         * should account for stat + syst
        //         * systs should account for: extrap. from anti-iso to iso region,
        //           possible difference between ratio in low mT and high mT (overkill?)
        //  - [x] lnNs for the W+jets OS/SS ratio
        //         * should account for stat only if not being accounted for with bbb,
        //           i.e. because the OS/SS ratio was measured with a relaxed selection
        //         * systs should account for: changes in low/high mT and OS/SS due to JES
        //           and btag (if relevant); OS/SS being wrong in the MC (from enriched data?);
        //           low/high mT being wrong in the MC (fake rate dependence?)

        // Going to use the regex filtering to select the right subset of
        // categories for each rateParam
        cb.SetFlag("filters-use-regex", true);
        //      for (auto bin : cb_sig.cp().channel({"et", "mt"}).bin_set()) {
        // Regex that matches, e.g. mt_nobtag or mt_nobtag_X



        cb.cp().bin({"mt_inclusive","mt_QCD_inclusive_cr","mt_W_inclusive_cr"}).process({"W"}).AddSyst(cb, "rate_W_cr_inclusive_mt", "rateParam", SystMap<>::init(1.0));
        cb.cp().bin({"et_inclusive","et_QCD_inclusive_cr","et_W_inclusive_cr"}).process({"W"}).AddSyst(cb, "rate_W_cr_inclusive_et", "rateParam", SystMap<>::init(1.0));

        cb.cp().bin({"mt_inclusive","mt_W_inclusive_cr","mt_QCD_inclusive_cr"}).process({"QCD"}).AddSyst(cb, "rate_QCD_cr_inclusive_mt", "rateParam", SystMap<>::init(1.0));
        cb.cp().bin({"et_inclusive","et_W_inclusive_cr","et_QCD_inclusive_cr"}).process({"QCD"}).AddSyst(cb, "rate_QCD_cr_inclusive_et", "rateParam", SystMap<>::init(1.0));

        cb.cp().bin({"tt_inclusive","tt_QCD_inclusive_cr"}).process({"QCD"}).AddSyst(cb, "rate_QCD_cr_inclusive_tt", "rateParam", SystMap<>::init(1.0));
        //uncomment me for QCD in W CR 


        /////////////////
        // Systematics //
        /////////////////

        // Should set a sensible range for our rateParams
        for (auto sys : cb.cp().syst_type({"rateParam"}).syst_name_set()) {
            cb.GetParameter(sys)->set_range(0.0, 5.0);
        }
        cb.SetFlag("filters-use-regex", false);
    }


    for (string chn : chns) {
        string file = aux_shapes + "xtt_" + chn +
            ".inputs-13TeV.root";
        //".inputs-13TeV-met.root";
        cb.cp().channel({chn}).backgrounds().ExtractShapes(
                file, "$BIN/$PROCESS", "$BIN/$PROCESS_$SYSTEMATIC");
        cb.cp().channel({chn}).signals().ExtractShapes(
                file, "$BIN/$PROCESS$MASS", "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    }
    cout << ">> Scaling signal process rates...\n";
    map<string, TGraph> xs;

    if (model=="Zprime"){ //Do not scale baryonci yet 
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
    }
    /*else if (model=="Baryonic"){ //Do not scale baryonci yet 
      for (string const& p : sig_procs) {
    // Get the table of xsecs vs mass for process "p" and era "e":
    cout << ">>>> Scaling for process " << p << "and signalMass " << signalMass <<" \n";
    cb.cp().process({p}).ForEachProc([&](ch::Process *proc) {
    std::string mass = proc->mass(); 
    int n = std::stoi(signalMass);
    int m = std::stoi(mass);
    cout << ">>>> Scaling for process " << p << "with mass "<<mass<<" \n";
    xs["xtt"+mass] = ch::TGraphFromTable(input_dir+"/xsecs_brs/"+model+"/xtt_monoH_"+signalMass+".txt", "mA", "br");
    proc->set_rate(1000000000*proc->rate() * xs["xtt"+mass].Eval(m));
    cout << ">>>> Scaling for model " << model << "with mass "<<mass<<" and Signal mass "<<n<< "\n";
    cout << ">>>> Scaling is "<< xs["xtt"+mass].Eval(m)<< "\n";
    //proc->set_rate(proc->rate() * xs["xtt"+mass].Eval(300));
    });
    }
    }*/

    if (do1Bin){
        //Now works!
        //cb.cp().FilterAll(BinIsNotControlRegion).ForEachProc(To1Bin<ch::Process>);
        //cb.cp().FilterAll(BinIsNotControlRegion).ForEachObs(To1Bin<ch::Observation>);

        cb.cp().FilterAll(BinIsNotControlRegion).ForEachProc(MergeBins<ch::Process>);
        cb.cp().FilterAll(BinIsNotControlRegion).ForEachObs(MergeBins<ch::Observation>);
    }

    //! [part8]
    auto bbb = ch::BinByBinFactory()
    .SetAddThreshold(0.05) //0.1
    //.SetMergeThreshold(0.8) //0.5
    .SetFixNorm(false)
    .SetVerbosity(1);
    //bbb.MergeBinErrors(cb.cp().backgrounds().FilterProcs(BinIsControlRegion));
    bbb.AddBinByBin(cb.cp().backgrounds().FilterProcs(BinIsControlRegion), cb);

    auto bbb_ctl = ch::BinByBinFactory()
        .SetPattern("CMS_$ANALYSIS_$BIN_$ERA_$PROCESS_bin_$#")
        .SetAddThreshold(0.1)
        //.SetMergeThreshold(0.5)
        .SetFixNorm(false)  // contrary to signal region, bbb *should* change yield here?
        //.SetFixNorm(true)  // contrary to signal region, bbb *should* change yield here?
        .SetVerbosity(1);
    // Will merge but only for non W and QCD processes, to be on the safe side
    bbb_ctl.AddBinByBin(cb.cp().backgrounds().FilterProcs(BinIsNotWControlRegion), cb);

    //bbb_ctl.AddBinByBin(cb.cp().backgrounds().FilterProcs(BinIsNotControlRegion), cb);
    //bbb_ctl.MergeBinErrors(cb.cp().process({"QCD", "W"}, false).FilterProcs(BinIsNotControlRegion));
    //bbb_ctl.MergeBinErrors(cb.cp().process({"QCD", "W"}, false).FilterProcs(BinIsNotControlRegion));
    cout << " done\n";
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
    writer.WriteCards("output/xtt_cards/"+model+signalMass+"A"+mass+"/cmb", cb);
    for (auto chn : chns) {
        writer.WriteCards("output/xtt_cards/"+model+signalMass+"A"+mass+"/"+chn, cb.cp().channel({chn}));
    }

    if (control_region > 0){

        for (auto chn : chns) {

            if (ch::contains({"et", "mt"}, chn)) {

                cb.cp().channel({chn}).bin_id({10}).mass({"$MASS", "*"}).WriteDatacard("output/xtt_cards/"+model+signalMass+"A"+mass+"/cmb/"+mass+ "/xtt_"+chn+"_10_13TeV.txt", "output/xtt_cards/"+model+signalMass+"A"+mass+"/cmb/"+mass+ "/xtt_input_"+chn+"_10.root");
                cb.cp().channel({chn}).bin_id({11}).mass({"$MASS", "*"}).WriteDatacard("output/xtt_cards/"+model+signalMass+"A"+mass+"/cmb/"+mass+ "/xtt_"+chn+"_11_13TeV.txt", "output/xtt_cards/"+model+signalMass+"A"+mass+"/cmb/"+mass+"/xtt_input_"+chn+"_11.root");

                cb.cp().channel({chn}).bin_id({10}).mass({"$MASS", "*"}).WriteDatacard("output/xtt_cards/"+model+signalMass+"A"+mass+"/"+chn+"/"+mass+ "/xtt_"+chn+"_10_13TeV.txt", "output/xtt_cards/"+model+signalMass+"A"+mass+"/"+chn+"/"+mass+ "/xtt_input_"+chn+"_10.root");
                cb.cp().channel({chn}).bin_id({11}).mass({"$MASS", "*"}).WriteDatacard("output/xtt_cards/"+model+signalMass+"A"+mass+"/"+chn+"/"+mass+ "/xtt_"+chn+"_11_13TeV.txt", "output/xtt_cards/"+model+signalMass+"A"+mass+"/"+chn+"/"+mass+"/xtt_input_"+chn+"_11.root");



            } // end et mt
            if (ch::contains({"tt"}, chn)) {
                cb.cp().channel({chn}).bin_id({11}).mass({"$MASS", "*"}).WriteDatacard("output/xtt_cards/"+model+signalMass+"A"+mass+"/cmb/"+mass+ "/xtt_"+chn+"_11_13TeV.txt", "output/xtt_cards/"+model+signalMass+"A"+mass+"/cmb/"+mass+ "/xtt_input_"+chn+"_11.root");
                cb.cp().channel({chn}).bin_id({11}).mass({"$MASS", "*"}).WriteDatacard("output/xtt_cards/"+model+signalMass+"A"+mass+"/"+chn+"/"+mass+ "/xtt_"+chn+"_11_13TeV.txt", "output/xtt_cards/"+model+signalMass+"A"+mass+"/"+chn+"/"+mass+ "/xtt_input_"+chn+"_11.root");
            }
        } // end CR
    }
    }
