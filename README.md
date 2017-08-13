# MonoHlimits
make limits for monoh tautau datacards

# instuctions for download
```
cmsrel CMSSW_7_4_7
cd CMSSW_7_4_7/src/
cmsenv
git cms-init
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
git clone https://github.com/cms-analysis/CombineHarvester.git CombineHarvester #combine harvester integration is underway 
mkdir Analysis
git clone git@github.com:USER/MonoHlimits.git Analysis/MonoHlimits 
```

Combine hard to set up. Always make a clean build, as scram doesn't always see updates to src/LinkDef.h. See https://twiki.cern.ch/twiki/bin/viewauth/CMS/SWGuideHiggsAnalysisCombinedLimit for more
```
cd $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit
git fetch origin
git checkout v6.2.1
scramv1 b clean; scramv1 b 
```

To build the package
```
cd $CMSSW_BASE/src/Analysis/MonoHlimits/
export USER_CXXFLAGS="-Wno-error=unused-but-set-variable -Wno-error=sign-compare"
source setup.sh
scram b -j8
```



to make plots make a folder in data 
```
mkdir Analysis/MonoHlimits/data/2017Early
#cp datacards to Early2017 folder name as desired. 
# file expects xtt_et_inputs-13TeV-mt.root  xtt_mt_inputs-13TeV-mt.root  xtt_tt_inputs-13TeV-mt.root
```


To produce limits:
```
cd Analysis/MonoHlimits/
source runBaryonic.sh
source run2HDM.sh 
``` 


To Produce plots in a new CMSSW release 
```
cmsrel CMSSW_9_0_0_pre4
cd  CMSSW_9_0_0_pre4/src/
cmsenv
wget 
```
