#!/bin/sh
XTT_CR --mass="300" --signalMass="1200"  control_region=1
cd output/xtt_cards/

combineTool.py -M T2W -i Zprime*/*/* -o workspace.root  --parallel 4
combine -M MaxLikelihoodFit --saveNormalizations --saveWithUncertainties --preFitValue 0 Zprime1200A300/cmb/300/workspace.root 
python $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/test/mlfitNormsToText.py -u mlfit.root
