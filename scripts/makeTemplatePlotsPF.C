#include "makePostfit.C"

void makeTemplatePlotsPF(){

   makeLTauStack("muTau_mt_postfit","xtt_postfit_shapes.root","xtt_mt_1_13TeV_postfit",0,"Total m_{T}","GeV",false,"#tau_{#mu}#tau_{h}","postfit",true,false,true);
   makeLTauStack("muTau_mt_prefit","xtt_postfit_shapes.root","xtt_mt_1_13TeV_prefit",0,"Total m_{T}","GeV",false,"#tau_{#mu}#tau_{h}","prefit",true,false,true);
   makeLTauStack("muTau_mt_W_postfit","xtt_postfit_shapes.root","xtt_mt_10_13TeV_postfit",0,"Total m_{T}","GeV",false,"#tau_{#mu}#tau_{h}","postfit",false,false,true);
   makeLTauStack("muTau_mt_W_prefit","xtt_postfit_shapes.root","xtt_mt_10_13TeV_prefit",0,"Total m_{T}","GeV",false,"#tau_{#mu}#tau_{h}","prefit",false,false,true);
   makeLTauStack("muTau_mt_QCD_postfit","xtt_postfit_shapes.root","xtt_mt_11_13TeV_postfit",0,"Total m_{T}","GeV",false,"#tau_{#mu}#tau_{h}","postfit",false,false,true);
   makeLTauStack("muTau_mt_QCD_prefit","xtt_postfit_shapes.root","xtt_mt_11_13TeV_prefit",0,"Total m_{T}","GeV",false,"#tau_{#mu}#tau_{h}","prefit",false,false,true);

   makeLTauStack("eleTau_mt_W_postfit","xtt_postfit_shapes.root","xtt_et_10_13TeV_postfit",0,"Total m_{T}","GeV",false,"#tau_{e}#tau_{h}","postfit",false,false,true);
   makeLTauStack("eleTau_mt_W_prefit","xtt_postfit_shapes.root","xtt_et_10_13TeV_prefit",0,"Total m_{T}","GeV",false,"#tau_{e}#tau_{h}","prefit",false,false,true);
   makeLTauStack("eleTau_mt_QCD_postfit","xtt_postfit_shapes.root","xtt_et_11_13TeV_postfit",0,"Total m_{T}","GeV",false,"#tau_{e}#tau_{h}","postfit",false,false,true);
   makeLTauStack("eleTau_mt_QCD_prefit","xtt_postfit_shapes.root","xtt_et_11_13TeV_prefit",0,"Total m_{T}","GeV",false,"#tau_{e}#tau_{h}","prefit",false,false,true);
   makeLTauStack("eleTau_mt_postfit","xtt_postfit_shapes.root","xtt_et_1_13TeV_postfit",0,"Total m_{T}","GeV",false,"#tau_{e}#tau_{h}","postfit",true,false,true);
   makeLTauStack("eleTau_mt_prefit","xtt_postfit_shapes.root","xtt_et_1_13TeV_prefit",0,"Total m_{T}","GeV",false,"#tau_{e}#tau_{h}","prefit",true,false,true);

   makeLTauStack("tauTau_mt_postfit","xtt_postfit_shapes.root","xtt_tt_1_13TeV_postfit",0,"Total m_{T}","GeV",false,"#tau_{h}#tau_{h}","postfit",true,false,true);
   makeLTauStack("tauTau_mt_prefit","xtt_postfit_shapes.root","xtt_tt_1_13TeV_prefit",0,"Total m_{T}","GeV",false,"#tau_{h}#tau_{h}","prefit",true,false,true);
   makeLTauStack("tauTau_mt_QCD_postfit","xtt_postfit_shapes.root","xtt_tt_11_13TeV_postfit",0,"Total m_{T}","GeV",false,"#tau_{h}#tau_{h}","postfit",false,false,true);
   makeLTauStack("tauTau_mt_QCD_prefit","xtt_postfit_shapes.root","xtt_tt_11_13TeV_prefit",0,"Total m_{T}","GeV",false,"#tau_{h}#tau_{h}","prefit",false,false,true);

}
