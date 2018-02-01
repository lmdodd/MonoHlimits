import os
import math
import ROOT
from array import array
import re
import json
import types

doFillAvg = 1
doFillAvgAll = 1

doFillFit = 0
doFillFitAll = 0

from CMS_lumi import CMS_lumi
import plotting_interp as plot
ROOT.gROOT.SetBatch(ROOT.kTRUE)
plot.ModTDRStyle()

canv = ROOT.TCanvas('test', 'test')
canv.SetLogz()

def SetMyPalette():
    alpha = 1
    nRGBs = 9
    stops = array(
        'd', [0.0000, 0.1250, 0.2500, 0.3750, 0.5000, 0.6250, 0.7500, 0.8750, 1.0000])
    red = array(
        'd', ([0./255.,  50./255.,  130./255.,  180./255., 200./255.,  215./255., 230./255., 240./255., 255./255.]))
    green = array(
        'd', ([0./255.,  50./255.,  130./255.,  180./255., 200./255.,  215./255., 230./255., 240./255., 255./255.]))
    blue = array(
        'd', ([255./255., 255./255., 255./255., 255./255., 255./255., 255./255., 255./255., 255./255., 255./255.]))
    ROOT.TColor.CreateGradientColorTable(nRGBs, stops, red, green, blue, 255, alpha)

SetMyPalette()
ROOT.gStyle.SetNumberContours(255)
ROOT.gStyle.SetOptStat(0)

A=[300,325,350,375,400,425,450,475,500,525,550,575,600,625,650,675,700,725,750,775,800,825,850,875,900,925,950,975]
Z=[600,650,700,750,800,850,900,950,1000,1050,1100,1150,1200,1250,1300,1350,1400,1450,1500,1550,1600,1650,1700,1750,1800,1850,1900,1950,2000,2050,2100,2150,2200,2250,2300,2350,2400,2450,2500]

limitPlot = ROOT.TH2F("lplot","lplot",39,600,2550,28,300,1000)
limitPlotObs = ROOT.TH2F("lplotObs","lplotObs",39,600,2550,28,300,1000)
limitPlotUp = ROOT.TH2F("lplotU","lplotU",39,600,2550,28,300,1000)
limitPlotDown = ROOT.TH2F("lplotDown","lplotDown",39,600,2550,28,300,1000)
limitPlotUp2 = ROOT.TH2F("lplotU2","lplotU2",39,600,2550,28,300,1000)
limitPlotDown2 = ROOT.TH2F("lplotDown2","lplotDown2",39,600,2550,28,300,1000)

limitPlotObs.GetXaxis().SetTitle("M_{Z'} [GeV]")
limitPlotObs.GetYaxis().SetTitle("M_{A_{0}} [GeV]")


i=1
for a in A:
    j=1
    for z in Z:
        data = {}
        filename='Zprime'+str(z)+'A'+str(a)+'.json'
        filenameObs='Zprime'+str(z)+'A'+str(a)+'.json'
        #print 'Using filename ' 
        #print filename
        if os.path.isfile(filename):
           with open(filename) as jsonfile:
              data = json.load(jsonfile)
              for key in data:
                  limitPlot.SetBinContent(j,i,data[key][u'exp0'])
                  limitPlotUp.SetBinContent(j,i,data[key][u'exp+1'])
                  limitPlotDown.SetBinContent(j,i,data[key][u'exp-1'])
                  limitPlotUp2.SetBinContent(j,i,data[key][u'exp+2'])
                  limitPlotDown2.SetBinContent(j,i,data[key][u'exp-2'])
                  limitPlotObs.SetBinContent(j,i,data[key][u'obs'])

        j=j+1
    i=i+1

if doFillAvg:
    i=1
    for a in A:
        j=1
        for z in Z:
            print "i: {0}   A: {1}   j: {2}   Z: {3}  Limit: {4}   ".format(i,a,j,z,str(limitPlotObs.GetBinContent(j,i)))
            binVal = str(limitPlotObs.GetBinContent(j,i))
            if binVal == "0.0":
                print " OBSERVED back: {0}   forward: {1}   down: {2}   up: {3}   ".format(str(limitPlotObs.GetBinContent(j-1,i)),str(limitPlotObs.GetBinContent(j+1,i)),str(limitPlotObs.GetBinContent(j,i-1)),str(limitPlotObs.GetBinContent(j,i+1)))
                avg = 0.0
                div = 0.0
                back = limitPlotObs.GetBinContent(j-1,i)
                if back != 0.0:
                    avg += back
                    div += 1
                forward = limitPlotObs.GetBinContent(j+1,i)
                if forward != 0.0:
                    avg += forward
                    div += 1
                down = limitPlotObs.GetBinContent(j,i-1)
                if down != 0.0:
                    avg += down
                    div += 1
                up = limitPlotObs.GetBinContent(j,i+1)
                if up != 0.0:
                    avg += up
                    div += 1
                avg = avg/div
                print "avg: " + str(avg)
                limitPlotObs.SetBinContent(j,i,avg)
                
                if doFillAvgAll:
                    print " EXP back: {0}   forward: {1}   down: {2}   up: {3}   ".format(str(limitPlot.GetBinContent(j-1,i)),str(limitPlot.GetBinContent(j+1,i)),str(limitPlot.GetBinContent(j,i-1)),str(limitPlot.GetBinContent(j,i+1)))
                    avg = 0.0
                    div = 0.0
                    back = limitPlot.GetBinContent(j-1,i)
                    if back != 0.0:
                        avg += back
                        div += 1
                    forward = limitPlot.GetBinContent(j+1,i)
                    if forward != 0.0:
                        avg += forward
                        div += 1
                    down = limitPlot.GetBinContent(j,i-1)
                    if down != 0.0:
                        avg += down
                        div += 1
                    up = limitPlot.GetBinContent(j,i+1)
                    if up != 0.0:
                        avg += up
                        div += 1
                    avg = avg/div
                    print "avg: " + str(avg)
                    limitPlot.SetBinContent(j,i,avg)
                    
                    print " EXP UP back: {0}   forward: {1}   down: {2}   up: {3}   ".format(str(limitPlotUp.GetBinContent(j-1,i)),str(limitPlotUp.GetBinContent(j+1,i)),str(limitPlotUp.GetBinContent(j,i-1)),str(limitPlotUp.GetBinContent(j,i+1)))
                    avg = 0.0
                    div = 0.0
                    back = limitPlotUp.GetBinContent(j-1,i)
                    if back != 0.0:
                        avg += back
                        div += 1
                    forward = limitPlotUp.GetBinContent(j+1,i)
                    if forward != 0.0:
                        avg += forward
                        div += 1
                    down = limitPlotUp.GetBinContent(j,i-1)
                    if down != 0.0:
                        avg += down
                        div += 1
                    up = limitPlotUp.GetBinContent(j,i+1)
                    if up != 0.0:
                        avg += up
                        div += 1
                    avg = avg/div
                    print "avg: " + str(avg)
                    limitPlotUp.SetBinContent(j,i,avg)
                    
                    print " EXP DOWN back: {0}   forward: {1}   down: {2}   up: {3}   ".format(str(limitPlotDown.GetBinContent(j-1,i)),str(limitPlotDown.GetBinContent(j+1,i)),str(limitPlotDown.GetBinContent(j,i-1)),str(limitPlotDown.GetBinContent(j,i+1)))
                    avg = 0.0
                    div = 0.0
                    back = limitPlotDown.GetBinContent(j-1,i)
                    if back != 0.0:
                        avg += back
                        div += 1
                    forward = limitPlotDown.GetBinContent(j+1,i)
                    if forward != 0.0:
                        avg += forward
                        div += 1
                    down = limitPlotDown.GetBinContent(j,i-1)
                    if down != 0.0:
                        avg += down
                        div += 1
                    up = limitPlotDown.GetBinContent(j,i+1)
                    if up != 0.0:
                        avg += up
                        div += 1
                    avg = avg/div
                    print "avg: " + str(avg)
                    limitPlotDown.SetBinContent(j,i,avg)
                    
                    print " EXP 2 UP back: {0}   forward: {1}   down: {2}   up: {3}   ".format(str(limitPlotUp2.GetBinContent(j-1,i)),str(limitPlotUp2.GetBinContent(j+1,i)),str(limitPlotUp2.GetBinContent(j,i-1)),str(limitPlotUp2.GetBinContent(j,i+1)))
                    avg = 0.0
                    div = 0.0
                    back = limitPlotUp2.GetBinContent(j-1,i)
                    if back != 0.0:
                        avg += back
                        div += 1
                    forward = limitPlotUp2.GetBinContent(j+1,i)
                    if forward != 0.0:
                        avg += forward
                        div += 1
                    down = limitPlotUp2.GetBinContent(j,i-1)
                    if down != 0.0:
                        avg += down
                        div += 1
                    up = limitPlotUp2.GetBinContent(j,i+1)
                    if up != 0.0:
                        avg += up
                        div += 1
                    avg = avg/div
                    print "avg: " + str(avg)
                    limitPlotUp2.SetBinContent(j,i,avg)
                    
                    print " EXP 2 DOWN back: {0}   forward: {1}   down: {2}   up: {3}   ".format(str(limitPlotDown2.GetBinContent(j-1,i)),str(limitPlotDown2.GetBinContent(j+1,i)),str(limitPlotDown2.GetBinContent(j,i-1)),str(limitPlotDown2.GetBinContent(j,i+1)))
                    avg = 0.0
                    div = 0.0
                    back = limitPlotDown2.GetBinContent(j-1,i)
                    if back != 0.0:
                        avg += back
                        div += 1
                    forward = limitPlotDown2.GetBinContent(j+1,i)
                    if forward != 0.0:
                        avg += forward
                        div += 1
                    down = limitPlotDown2.GetBinContent(j,i-1)
                    if down != 0.0:
                        avg += down
                        div += 1
                    up = limitPlotDown2.GetBinContent(j,i+1)
                    if up != 0.0:
                        avg += up
                        div += 1
                    avg = avg/div
                    print "avg: " + str(avg)
                    limitPlotDown2.SetBinContent(j,i,avg)
            j=j+1
        i=i+1

testShapes = ROOT.TFile("testShapes.root","RECREATE")
if doFillFit:
    for i in range (1,limitPlotObs.GetYaxis().GetNbins()+1):
        print "Y Bin: " + str(i)
        j = 1
        while j < limitPlotObs.GetXaxis().GetNbins():
            k = 1
            if str(limitPlotObs.GetBinContent(j,i)) == "0.0":
                print "GAP: "
                while limitPlotObs.GetBinContent(j+k,i) == 0.0:
                    k += 1
                tempPlot = ROOT.TH1F("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),"tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),2+k,0,2+k)
                tempFit = ROOT.TF1("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k),"[0]+[1]*x",0,2+k)
                m = 1
                for l in range(j-1,j+k+1):
                    print limitPlotObs.GetBinContent(l,i)
                    tempPlot.SetBinContent(m,limitPlotObs.GetBinContent(l,i))
                    m+=1
                tempPlot.Fit("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                m = 2
                for l in range(j,j+k):
                    print tempFit.Eval(tempPlot.GetBinCenter(m))
                    limitPlotObs.SetBinContent(l,i,tempFit.Eval(tempPlot.GetBinCenter(m)))
                    print "Setting Value: " + str(tempFit.Eval(tempPlot.GetBinCenter(m)))
                    m+=1
                tempPlot.Write("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                tempFit.Write("tempPlotFit_"+str(i)+"_"+str(j)+"_"+str(k))
            j=j+k
    if doFillFitAll:
        for i in range (1,limitPlot.GetYaxis().GetNbins()+1):
            print "Y Bin: " + str(i)
            j = 1
            while j < limitPlot.GetXaxis().GetNbins():
                k = 1
                if str(limitPlot.GetBinContent(j,i)) == "0.0":
                    print "GAP: "
                    while limitPlot.GetBinContent(j+k,i) == 0.0:
                        k += 1
                    tempPlot = ROOT.TH1F("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),"tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),2+k,0,2+k)
                    tempFit = ROOT.TF1("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k),"[0]+[1]*x",0,2+k)
                    m = 1
                    for l in range(j-1,j+k+1):
                        print limitPlot.GetBinContent(l,i)
                        tempPlot.SetBinContent(m,limitPlot.GetBinContent(l,i))
                        m+=1
                    tempPlot.Fit("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                    m = 2
                    for l in range(j,j+k):
                        print tempFit.Eval(tempPlot.GetBinCenter(m))
                        limitPlot.SetBinContent(l,i,tempFit.Eval(tempPlot.GetBinCenter(m)))
                        print "Setting Value: " + str(tempFit.Eval(tempPlot.GetBinCenter(m)))
                        m+=1
                    tempPlot.Write("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                    tempFit.Write("tempPlotFit_"+str(i)+"_"+str(j)+"_"+str(k))
                j=j+k
        for i in range (1,limitPlotUp.GetYaxis().GetNbins()+1):
            print "Y Bin: " + str(i)
            j = 1
            while j < limitPlotUp.GetXaxis().GetNbins():
                k = 1
                if str(limitPlotUp.GetBinContent(j,i)) == "0.0":
                    print "GAP: "
                    while limitPlotUp.GetBinContent(j+k,i) == 0.0:
                        k += 1
                    tempPlot = ROOT.TH1F("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),"tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),2+k,0,2+k)
                    tempFit = ROOT.TF1("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k),"[0]+[1]*x",0,2+k)
                    m = 1
                    for l in range(j-1,j+k+1):
                        print limitPlotUp.GetBinContent(l,i)
                        tempPlot.SetBinContent(m,limitPlotUp.GetBinContent(l,i))
                        m+=1
                    tempPlot.Fit("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                    m = 2
                    for l in range(j,j+k):
                        print tempFit.Eval(tempPlot.GetBinCenter(m))
                        limitPlotUp.SetBinContent(l,i,tempFit.Eval(tempPlot.GetBinCenter(m)))
                        print "Setting Value: " + str(tempFit.Eval(tempPlot.GetBinCenter(m)))
                        m+=1
                    tempPlot.Write("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                    tempFit.Write("tempPlotFit_"+str(i)+"_"+str(j)+"_"+str(k))
                j=j+k
        for i in range (1,limitPlotDown.GetYaxis().GetNbins()+1):
            print "Y Bin: " + str(i)
            j = 1
            while j < limitPlotDown.GetXaxis().GetNbins():
                k = 1
                if str(limitPlotDown.GetBinContent(j,i)) == "0.0":
                    print "GAP: "
                    while limitPlotDown.GetBinContent(j+k,i) == 0.0:
                        k += 1
                    tempPlot = ROOT.TH1F("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),"tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),2+k,0,2+k)
                    tempFit = ROOT.TF1("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k),"[0]+[1]*x",0,2+k)
                    m = 1
                    for l in range(j-1,j+k+1):
                        print limitPlotDown.GetBinContent(l,i)
                        tempPlot.SetBinContent(m,limitPlotDown.GetBinContent(l,i))
                        m+=1
                    tempPlot.Fit("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                    m = 2
                    for l in range(j,j+k):
                        print tempFit.Eval(tempPlot.GetBinCenter(m))
                        limitPlotDown.SetBinContent(l,i,tempFit.Eval(tempPlot.GetBinCenter(m)))
                        print "Setting Value: " + str(tempFit.Eval(tempPlot.GetBinCenter(m)))
                        m+=1
                    tempPlot.Write("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                    tempFit.Write("tempPlotFit_"+str(i)+"_"+str(j)+"_"+str(k))
                j=j+k
        for i in range (1,limitPlotUp2.GetYaxis().GetNbins()+1):
            print "Y Bin: " + str(i)
            j = 1
            while j < limitPlotUp2.GetXaxis().GetNbins():
                k = 1
                if str(limitPlotUp2.GetBinContent(j,i)) == "0.0":
                    print "GAP: "
                    while limitPlotUp2.GetBinContent(j+k,i) == 0.0:
                        k += 1
                    tempPlot = ROOT.TH1F("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),"tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),2+k,0,2+k)
                    tempFit = ROOT.TF1("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k),"[0]+[1]*x",0,2+k)
                    m = 1
                    for l in range(j-1,j+k+1):
                        print limitPlotUp2.GetBinContent(l,i)
                        tempPlot.SetBinContent(m,limitPlotUp2.GetBinContent(l,i))
                        m+=1
                    tempPlot.Fit("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                    m = 2
                    for l in range(j,j+k):
                        print tempFit.Eval(tempPlot.GetBinCenter(m))
                        limitPlotUp2.SetBinContent(l,i,tempFit.Eval(tempPlot.GetBinCenter(m)))
                        print "Setting Value: " + str(tempFit.Eval(tempPlot.GetBinCenter(m)))
                        m+=1
                    tempPlot.Write("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                    tempFit.Write("tempPlotFit_"+str(i)+"_"+str(j)+"_"+str(k))
                j=j+k
        for i in range (1,limitPlotDown2.GetYaxis().GetNbins()+1):
            print "Y Bin: " + str(i)
            j = 1
            while j < limitPlotDown2.GetXaxis().GetNbins():
                k = 1
                if str(limitPlotDown2.GetBinContent(j,i)) == "0.0":
                    print "GAP: "
                    while limitPlotDown2.GetBinContent(j+k,i) == 0.0:
                        k += 1
                    tempPlot = ROOT.TH1F("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),"tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k),2+k,0,2+k)
                    tempFit = ROOT.TF1("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k),"[0]+[1]*x",0,2+k)
                    m = 1
                    for l in range(j-1,j+k+1):
                        print limitPlotDown2.GetBinContent(l,i)
                        tempPlot.SetBinContent(m,limitPlotDown2.GetBinContent(l,i))
                        m+=1
                    tempPlot.Fit("tempFitGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                    m = 2
                    for l in range(j,j+k):
                        print tempFit.Eval(tempPlot.GetBinCenter(m))
                        limitPlotDown2.SetBinContent(l,i,tempFit.Eval(tempPlot.GetBinCenter(m)))
                        print "Setting Value: " + str(tempFit.Eval(tempPlot.GetBinCenter(m)))
                        m+=1
                    tempPlot.Write("tempPlotGAP_"+str(i)+"_"+str(j)+"_"+str(k))
                    tempFit.Write("tempPlotFit_"+str(i)+"_"+str(j)+"_"+str(k))
                j=j+k

testShapes.Close()

limitPlotObs.GetZaxis().SetRange(0,5000)
limitPlotObs.SetBarOffset(0.10)
limitPlotObs.Draw("COLZ")

limitPlotUp.SetMinimum(1);
limitPlotUp.SetContour(1);
limitPlotUp.SetLineWidth(2);
limitPlotUp.SetLineColor(8);
limitPlotUp.Draw("CONT3 SAME")

limitPlotDown.SetMinimum(1);
limitPlotDown.SetContour(1);
limitPlotDown.SetLineWidth(2);
limitPlotDown.SetLineColor(8);
limitPlotDown.Draw("CONT3 SAME")

limitPlotUp2.SetMinimum(1);
limitPlotUp2.SetContour(1);
limitPlotUp2.SetLineWidth(2);
limitPlotUp2.SetLineColor(5);
limitPlotUp2.Draw("CONT3 SAME")

limitPlotDown2.SetMinimum(1);
limitPlotDown2.SetContour(1);
limitPlotDown2.SetLineWidth(2);
limitPlotDown2.SetLineColor(5);
limitPlotDown2.Draw("CONT3 SAME")

limitPlotObsCopy = limitPlotObs.Clone()
limitPlotObsCopy.SetMinimum(1);
limitPlotObsCopy.SetContour(1);
limitPlotObsCopy.SetLineWidth(2);
limitPlotObsCopy.Draw("CONT3 SAME")

limitPlot.SetMinimum(1);
limitPlot.SetContour(1);
limitPlot.SetLineStyle(7);
limitPlot.SetLineWidth(3);
limitPlot.Draw("CONT3 SAME")

leg = ROOT.TLegend(.35,.75,.90,.90)
leg.SetBorderSize(0)
leg.SetFillColor(0)
leg.SetFillStyle(0)
leg.SetTextFont(42)
leg.SetTextSize(0.030)
leg.AddEntry(limitPlotObsCopy,"Observed Limit r = 1 (95% CL)","L")
leg.AddEntry(limitPlot,"Expected Limit r = 1 (95% CL)","L")
leg.AddEntry(limitPlotUp,"Expected Limit +/- \sigma r = 1 (95% CL)","L")
leg.AddEntry(limitPlotUp2,"Expected Limit +/- 2\sigma r = 1 (95% CL)","L")
leg.Draw()

CMS_lumi(canv,4,0)

limitPlot.SaveAs("test.root")
canv.Print("test.pdf")
