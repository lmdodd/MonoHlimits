#!/bin/bash
source scripts/make2HDM_CR_interp.sh
cp scripts/collect2HDMLimitsInterp.py output/xtt_cards/
cd output/xtt_cards/
combineTool.py -M T2W -i Zprime*/cmb/* -o workspace.root  --parallel 4 
#combineTool.py -M Asymptotic -d Zprime*/cmb/*/workspace.root -t -1 --there -n .limit --parallel 8
combineTool.py -M Asymptotic -d Zprime*/cmb/*/workspace.root --there -n .limit --parallel 8
python collect2HDMLimitsInterp.py
