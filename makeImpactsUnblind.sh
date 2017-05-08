#!bin/bash
#example commands to run impact plots, any signal directory is ok. 

combineTool.py -M Impacts -m 120 -d card.root  --doInitialFit --rMin -20 --rMax 20 --robustFit 1 --expectSignal=1  --parallel 8
combineTool.py -M Impacts -m 120 -d card.root  --robustFit 1 --rMin -20 --rMax 20  --doFits --expectSignal=1  --parallel 8
combineTool.py -M Impacts -m 120 -d card.root  -o impacts.json
plotImpacts.py -i impacts.json -o impacts
