import os
import math
import ROOT
from array import array
import re
import json
import types

import plotting as plot
ROOT.gROOT.SetBatch(ROOT.kTRUE)
plot.ModTDRStyle()

canv = ROOT.TCanvas('test', 'test')
#canv.SetLogz()

ROOT.gStyle.SetPalette(92) 
ROOT.gStyle.SetOptStat(0)

A=[1,50,150,500,1000]
Z=[10000,1000,500,100,10]

#dirs=['Baryonic1000A1','Baryonic1000A150','Baryonic500A150','Baryonic500A500']
#for d in dirs: 
#    cmd = "combineTool.py -M CollectLimits "+d+ "/*/*/*.limit.* -o "+d+".json"
#    os.system(cmd)

x =len(A)
y =len(Z)
limitPlot = ROOT.TH2F("lplot","lplot",x,0,x,y,0,y)
limitPlotUp = ROOT.TH2F("lplotU","lplotU",x,0,x,y,0,y)
limitPlotDown = ROOT.TH2F("lplotDown","lplotDown",x,0,x,y,0,y)

limitPlot.GetXaxis().SetTitle("M_{#Chi} [GeV]")
limitPlot.GetYaxis().SetTitle("M_{Z'} [GeV]")


i=0
for a in A:
    j=0
    for z in Z:
        data = {}
        filename='Baryonic'+str(z)+'A'+str(a)+'.json'
        print 'Using filename ' 
        print filename
        if os.path.isfile(filename):
           with open(filename) as jsonfile:
              data = json.load(jsonfile)
              for key in data:
                  print key
                  print data[key]
                  limitPlot.SetBinContent(i+1,j+1,data[key][u'exp0'])
                  limitPlotUp.SetBinContent(i+1,j+1,data[key][u'exp+1'])
                  limitPlotDown.SetBinContent(i+1,j+1,data[key][u'exp-1'])
                  print "Setting bin content %f,%f with value %f" %(i+1,j+1,data[key][u'exp0'])
        limitPlot.GetXaxis().SetBinLabel(i+1,str(A[i]))
        limitPlot.GetYaxis().SetBinLabel(j+1,str(Z[j]))
        j=j+1
    i=i+1

limitPlot.Draw("COL TEXT")
limitPlotUp.SetBarOffset(0.25)
limitPlotUp.SetMarkerColor(2)
limitPlotUp.Draw("TEXT SAME")
limitPlotDown.SetBarOffset(-0.25)
limitPlotDown.SetMarkerColor(2)
limitPlotDown.Draw("TEXT SAME")
limitPlot.SaveAs("test.root")
canv.Print("testBaryonic.pdf")
