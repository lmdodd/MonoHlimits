#!bin/bash
#example commands to run impact plots, any signal directory is ok. 

XTT_CR --mass="400" --signalMass="1200"  control_region=1
cp scripts/plot1DScan.py output/xtt_cards/
cp scripts/texName.json output/xtt_cards/
cd output/xtt_cards
combineTool.py -M T2W -i Zprime*/{et,mt,tt,cmb}/* -o workspace.root  --parallel 4

for CHANNEL in cmb; do
    combine -M MultiDimFit -m 400 --algo grid --points 101 --rMin -1 --rMax 3.0 Zprime1200A400/${CHANNEL}/400/workspace.root -n nominal  --minimizerAlgoForMinos Minuit2,Migrad --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP
    combine -M MultiDimFit --algo none --rMin -1 --rMax 3.0 Zprime1200A400/${CHANNEL}/400/workspace.root -m 400 -n bestfit --saveWorkspace  --minimizerAlgoForMinos Minuit2,Migrad --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP
done

combine -M MultiDimFit --algo grid --points 101 --rMin -1 --rMax 3.0 -m 400 -n stat \
    higgsCombinebestfit.MultiDimFit.mH400.root --snapshotName MultiDimFit --freezeNuisances all \
    --minimizerAlgoForMinos Minuit2,Migrad --X-rtd FITTER_NEW_CROSSING_ALGO --X-rtd FITTER_NEVER_GIVE_UP

python ./plot1DScan.py --main higgsCombinenominal.MultiDimFit.mH400.root \
    --POI r -o cms_output_freeze_All \
    --x-range 0,2.5 \
    --others 'higgsCombinestat.MultiDimFit.mH400.root:Freeze all:2' \
    --breakdown syst,stat

