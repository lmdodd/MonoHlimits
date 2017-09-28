# Base off ./HIG16006/input/mssm_protocol.txt

XTT_CR --mass="300" --signalMass="1200"  control_region=1
cd output/xtt_cards/
combineTool.py -M T2W -i Zprime*/*/* -o workspace.root  --parallel 4
cd Zprime1200A300/
# We look at the Goodness of Fit for three different algorithms. 
# The saturated model (saturated), Anderson-Darling (AD) and 
# Kolmogorow-Smirnow (KS). For the AD and KS it is sufficient to 
# run the fits for the combined cards as the test-statitic for 
# the individual channels can be extracted from these results. 
# For the saturated model it is necessary to run them independtly 
# of each other.


###  THESE COMMANDS PRODUCE UNBLINDED RESULTS!!!  ###
echo ""
echo ""
echo "THESE COMMANDS PRODUCE UNBLINDED RESULTS!!!"
echo ""
echo ""


## Do Anderson Darling first
ALGO=AD
combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 300 --there -d cmb/300/workspace.root -n ".$ALGO.toys" --fixedSignalStrength=1 -t 25 -s 0:19:1 --parallel 12
combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 300 --there -d cmb/300/workspace.root -n ".$ALGO" --fixedSignalStrength=1

combineTool.py -M CollectGoodnessOfFit --input cmb/300/higgsCombine.${ALGO}.GoodnessOfFit.mH300.root cmb/300/higgsCombine.${ALGO}.toys.GoodnessOfFit.mH300.*.root -o collectGoodness_${ALGO}.json
#
python ../../../../../CombineHarvester/CombineTools/scripts/plotGof.py --statistic ${ALGO} --mass 300.0 collectGoodness_${ALGO}.json --title-right="35.9 fb^{-1} (13 TeV)" --output='-AD'
#
#
#
#
#
## Do KS test
ALGO=KS
combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 300 --there -d cmb/300/workspace.root -n ".$ALGO.toys" --fixedSignalStrength=1 -t 25 -s 0:19:1 --parallel 12
combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 300 --there -d cmb/300/workspace.root -n ".$ALGO" --fixedSignalStrength=1

combineTool.py -M CollectGoodnessOfFit --input cmb/300/higgsCombine.${ALGO}.GoodnessOfFit.mH300.root cmb/300/higgsCombine.${ALGO}.toys.GoodnessOfFit.mH300.*.root -o collectGoodness_${ALGO}.json

python ../../../../../CombineHarvester/CombineTools/scripts/plotGof.py --statistic ${ALGO} --mass 300.0 collectGoodness_${ALGO}.json --title-right="35.9 fb^{-1} (13 TeV)" --output='-KS'





# Do Saturated
# For the saturated model run for each category seperatly
# We need to make indivual workspaces for each channel/bin
ALGO=saturated

for CHANNEL in et mt tt; do
    for BIN in 1 ; do
        echo "Saturated for ${CHANNEL} ${BIN}"
        combineTool.py -M T2W -i cmb/300/xtt_${CHANNEL}_${BIN}*.txt -o workspace_${CHANNEL}_${BIN}.root --parallel 2
        combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 300 --there -d cmb/300/workspace_${CHANNEL}_${BIN}.root -n ".$ALGO.toys"  -t 25 -s 0:19:1 --parallel 12
        combineTool.py -M GoodnessOfFit --algorithm $ALGO -m 300 --there -d cmb/300/workspace_${CHANNEL}_${BIN}.root -n ".$ALGO" 
        combineTool.py -M CollectGoodnessOfFit --input cmb/300/higgsCombine.saturated.GoodnessOfFit.mH300.root Zprime1200A300/cmb/300/higgsCombine.saturated.toys.GoodnessOfFit.mH300.*.root -o xtt_${CHANNEL}_${BIN}_saturated.json
    done
done


# Do saturated plotting separately to properly label
plotGof.py --statistic ${ALGO} --mass 300.0 xtt_et_1_saturated.json --title-right="35.9 fb^{-1} (13 TeV)" --output='et_1-Saturated' --title-left="e#tau_{h} "
plotGof.py --statistic ${ALGO} --mass 300.0 xtt_mt_1_saturated.json --title-right="35.9 fb^{-1} (13 TeV)" --output='mt_1-Saturated' --title-left="#mu#tau_{h} "
plotGof.py --statistic ${ALGO} --mass 300.0 xtt_tt_1_saturated.json --title-right="35.9 fb^{-1} (13 TeV)" --output='tt_1-Saturated' --title-left="#tau_{h}#tau_{h} "

