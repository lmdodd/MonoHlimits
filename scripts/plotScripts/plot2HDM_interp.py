import os
import math
import ROOT
from array import array
import re
import json
import types

import plotting_interp as plot
ROOT.gROOT.SetBatch(ROOT.kTRUE)
plot.ModTDRStyle()

canv = ROOT.TCanvas('test', 'test')
canv.SetLogz()

ROOT.gStyle.SetPalette(92) 
ROOT.gStyle.SetOptStat(0)

A=[300,325,350,375,400,425,450,475,500,525,550,575,600,625,650,675,700,725,750,775,800,825,850,875,900,925,950,975]
Z=[600,650,700,750,800,850,900,950,1000,1050,1100,1150,1200,1250,1300,1350,1400,1450,1500,1550,1600,1650,1700,1750,1800,1850,1900,1950,2000,2050,2150,2200,2250,2300,2350,2400,2450,2500]

limitPlot = ROOT.TH2F("lplot","lplot",38,0,38,28,0,28)
limitPlotObs = ROOT.TH2F("lplotObs","lplotObs",38,0,38,28,0,28)
limitPlotUp = ROOT.TH2F("lplotU","lplotU",38,0,38,28,0,28)
limitPlotDown = ROOT.TH2F("lplotDown","lplotDown",38,0,38,28,0,28)

limitPlot.GetXaxis().SetTitle("M_{Z} [GeV]")
limitPlot.GetYaxis().SetTitle("M_{A'} [GeV]")


i=0
for a in A:
    j=0
    for z in Z:
        data = {}
        filename='OBS/Zprime'+str(z)+'A'+str(a)+'.json'
        filenameObs='OBS/Zprime'+str(z)+'A'+str(a)+'.json'
        #print 'Using filename ' 
        #print filename
        if os.path.isfile(filename):
           with open(filename) as jsonfile:
              data = json.load(jsonfile)
              for key in data:
                  limitPlot.SetBinContent(j+1,i+1,data[key][u'exp0'])
                  limitPlotUp.SetBinContent(j+1,i+1,data[key][u'exp+1'])
                  limitPlotDown.SetBinContent(j+1,i+1,data[key][u'exp-1'])
                  #print "Setting bin content %f,%f with value %f" %(i+1,j+1,data[key][u'exp0'])
        if os.path.isfile(filenameObs):
           with open(filenameObs) as jsonfile:
              data = json.load(jsonfile)
              for key in data:
                  limitPlotObs.SetBinContent(j+1,i+1,data[key][u'obs'])
                  #print "Setting bin content %f,%f with value %f" %(i+1,j+1,data[key][u'exp0'])  
        if "00" in str(Z[j]):
            limitPlot.GetXaxis().SetBinLabel(j+1,str(Z[j]))
        if "00" in str(A[i]):
            limitPlot.GetYaxis().SetBinLabel(i+1,str(A[i]))
        j=j+1
    i=i+1

limitPlot.SetBarOffset(-0.10)
limitPlot.Draw("COL TEXT")
limitPlotUp.SetBarOffset(0.35)
limitPlotUp.SetMarkerColor(2)
limitPlotUp.Draw("TEXT SAME")
limitPlotObs.SetBarOffset(0.10)
limitPlotObs.SetMarkerColor(4)
limitPlotObs.Draw("TEXT SAME")
limitPlotDown.SetBarOffset(-0.35)
limitPlotDown.SetMarkerColor(2)
limitPlotDown.Draw("TEXT SAME")
limitPlot.SaveAs("test.root")
canv.Print("test.pdf")
