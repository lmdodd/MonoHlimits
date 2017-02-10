#!/bin/bash
source scripts/makeBaryonic.sh
cp scripts/collectBaryonicLimits.py output/xtt_cards/
cd output/xtt_cards
combineTool.py -M T2W -i Baryonic*/*/* -o workspace.root  --parallel 4 
combineTool.py -M Asymptotic -d Baryonic*/*/*/workspace.root -t -1 --there -n .limit --parallel 8 
python collectBaryonicLimits.py
