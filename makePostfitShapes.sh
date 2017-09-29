XTT_CR --mass="300" --signalMass="1200"  control_region=1
cp scripts/makePostfit.C output/xtt_cards/
cp scripts/makeTemplatePlotsPF.C output/xtt_cards/
cp scripts/CMS_lumi.* output/xtt_cards/
cp scripts/tdrstyle.* output/xtt_cards/
cd output/xtt_cards/

combineTool.py -M T2W -i Zprime*/*/* -o workspace.root  --parallel 4

combine -M MaxLikelihoodFit Zprime1200A300/cmb/300/workspace.root  --robustFit=1 --preFitValue=0. --X-rtd FITTER_NEW_CROSSING_ALGO --minimizerAlgoForMinos=Minuit2 --minimizerToleranceForMinos=0.1 --X-rtd FITTER_NEVER_GIVE_UP --X-rtd FITTER_BOUND --minimizerAlgo=Minuit2 --minimizerStrategy=0 --minimizerTolerance=0.1 --cminFallbackAlgo \"Minuit2,0:1.\"  --rMin -2 --rMax 2
PostFitShapesFromWorkspace -o xtt_postfit_shapes.root -m 300 -f mlfit.root:fit_s --postfit --sampling --print -d Zprime1200A300/cmb/300/combined.txt.cmb -w Zprime1200A300/cmb/300/workspace.root
root -l -b -q makeTemplatePlotsPF.C
