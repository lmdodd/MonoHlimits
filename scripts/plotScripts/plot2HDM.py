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
canv.SetLogz()

ROOT.gStyle.SetPalette(92) 
ROOT.gStyle.SetOptStat(0)

A=[300,400,500,600,700,800]
Z=[600,800,1000,1200,1400,1700,2000,2500]

dirs=['Zprime1000A300','Zprime1000A700','Zprime1200A500','Zprime1400A300','Zprime1400A700','Zprime1700A500','Zprime2000A300','Zprime2000A700','Zprime2500A500','Zprime600A300','Zprime800A500','Zprime1000A400','Zprime1000A800','Zprime1200A600','Zprime1400A400','Zprime1400A800','Zprime1700A600','Zprime2000A400','Zprime2000A800','Zprime2500A600','Zprime600A400','Zprime800A600','Zprime1000A500','Zprime1200A300','Zprime1200A700','Zprime1400A500','Zprime1700A300','Zprime1700A700','Zprime2000A500','Zprime2500A300','Zprime2500A700','Zprime800A300','Zprime1000A600','Zprime1200A400','Zprime1200A800','Zprime1400A600','Zprime1700A400','Zprime1700A800','Zprime2000A600','Zprime2500A400','Zprime2500A800','Zprime800A400']
#for d in dirs: 
#    cmd = "combineTool.py -M CollectLimits "+d+ "/*/*/*.limit.* -o "+d+".json"
#    os.system(cmd)

limitPlot = ROOT.TH2F("lplot","lplot",6,0,6,8,0,8)
limitPlotUp = ROOT.TH2F("lplotU","lplotU",6,0,6,8,0,8)
limitPlotDown = ROOT.TH2F("lplotDown","lplotDown",6,0,6,8,0,8)

limitPlot.GetXaxis().SetTitle("M_{A} [GeV]")
limitPlot.GetYaxis().SetTitle("M_{Z'} [GeV]")


i=0
for a in A:
    j=0
    for z in Z:
        data = {}
        filename='Zprime'+str(z)+'A'+str(a)+'.json'
        #print 'Using filename ' 
        #print filename
        if os.path.isfile(filename):
           with open(filename) as jsonfile:
              data = json.load(jsonfile)
              for key in data:
                  limitPlot.SetBinContent(i+1,j+1,data[key][u'exp0'])
                  limitPlotUp.SetBinContent(i+1,j+1,data[key][u'exp+1'])
                  limitPlotDown.SetBinContent(i+1,j+1,data[key][u'exp-1'])
                  #print "Setting bin content %f,%f with value %f" %(i+1,j+1,data[key][u'exp0'])
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
canv.Print("test.pdf")
