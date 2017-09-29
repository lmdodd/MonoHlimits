#!bin/bash
#example commands to run impact plots, any signal directory is ok. 

XTT_CR --mass="400" --signalMass="1200"  control_region=1
cd output/xtt_cards
combineTool.py -M T2W -i Zprime*/*/* -o workspace.root  --parallel 4
combineTool.py -M Impacts -d Zprime1200A400/cmb/400/workspace.root -m 400 --doInitialFit --rMin -5 --rMax 5 --robustFit 1 --expectSignal=0  --parallel 8
combineTool.py -M Impacts -d Zprime1200A400/cmb/400/workspace.root -m 400 --robustFit 1 --rMin -5 --rMax 5  --doFits --expectSignal=0 --parallel 8
combineTool.py -M Impacts -d Zprime1200A400/cmb/400/workspace.root -m 400 -o impacts.json
plotImpacts.py -i impacts.json -o impacts
