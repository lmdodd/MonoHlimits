pushd $CMSSW_BASE/src

cp Analysis/MonoHlimits/recipe/XTT.cpp CombineHarvester/CombineTools/bin/
cp Analysis/MonoHlmits/recipe/XTTBoost.cpp CombineHarvester/CombineTools/bin/
cp Analysis/MonoHlimits/recipe/BuildFile.xml CombineHarvester/CombineTools/bin/
cp Analysis/MonoHlimits/recipe/xtt_monoH_*txt CombineHarvester/CombineTools/input/xsecs_brs/

scram b -j8
