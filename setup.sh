pushd $CMSSW_BASE/src

cp Analysis/MonoHlimits/recipe/XTT.cpp CombineHarvester/CombineTools/bin/
cp Analysis/MonoHlimits/recipe/XTT_SS.cpp CombineHarvester/CombineTools/bin/
cp Analysis/MonoHlimits/recipe/XTT_LowMt.cpp CombineHarvester/CombineTools/bin/
cp Analysis/MonoHlimits/recipe/XTT_CR.cpp CombineHarvester/CombineTools/bin/

#inprogress
#cp Analysis/MonoHlmits/recipe/XTTBoost.cpp CombineHarvester/CombineTools/bin/ 

cp Analysis/MonoHlimits/recipe/BuildFile.xml CombineHarvester/CombineTools/bin/
cp -r Analysis/MonoHlimits/recipe/Baryonic/ CombineHarvester/CombineTools/input/xsecs_brs/
cp -r Analysis/MonoHlimits/recipe/Zprime/ CombineHarvester/CombineTools/input/xsecs_brs/

scram b -j8
pushd $CMSSW_BASE/src/Analysis/MonoHlimits
