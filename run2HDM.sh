#!/bin/bash
source scripts/make2HDM.sh
cp scripts/collect2HDMLimits.py output/xtt_cards/
cd output/xtt_cards/
combineTool.py -M T2W -i Zprime*/*/* -o workspace.root  --parallel 4 
combineTool.py -M Asymptotic -d Zprime*/*/*/workspace.root -t -1 --there -n .limit --parallel 8
python collect2HDMLimits.py
