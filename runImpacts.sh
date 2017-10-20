#!bin/bash
#example commands to run impact plots, any signal directory is ok. 

XTT_CR --mass="400" --signalMass="1200"  --control_region=1 --dobbb=1
cd output/xtt_cards
combineTool.py -M T2W -i Zprime*/{et,mt,tt,cmb}/* -o workspace.root  --parallel 4

#for CHANNEL in et mt tt cmb; do
for CHANNEL in cmb; do
    combineTool.py -M Impacts -d Zprime1200A400/${CHANNEL}/400/workspace.root -m 400  --doInitialFit --rMin -10 --rMax 10 --robustFit 1  --minimizerAlgoForMinos Minuit2,Migrad --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --parallel 24
    combineTool.py -M Impacts -d Zprime1200A400/${CHANNEL}/400/workspace.root -m 400  --robustFit 1 --rMin -10 --rMax 10  --doFits --minimizerAlgoForMinos Minuit2,Migrad --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP --parallel 24
    combineTool.py -M Impacts -d Zprime1200A400/${CHANNEL}/400/workspace.root -m 400 -o impacts_${CHANNEL}.json
    plotImpacts.py -i impacts_${CHANNEL}.json -o impacts_${CHANNEL}
done
