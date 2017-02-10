import os
A=[300,400,500,600,700,800]
Z=[600,800,1000,1200,1400,1700,2000,2500]

dirs=['Zprime1000A300','Zprime1000A700','Zprime1200A500','Zprime1400A300','Zprime1400A700','Zprime1700A500','Zprime2000A300','Zprime2000A700','Zprime2500A500','Zprime600A300','Zprime800A500','Zprime1000A400','Zprime1000A800','Zprime1200A600','Zprime1400A400','Zprime1400A800','Zprime1700A600','Zprime2000A400','Zprime2000A800','Zprime2500A600','Zprime600A400','Zprime800A600','Zprime1000A500','Zprime1200A300','Zprime1200A700','Zprime1400A500','Zprime1700A300','Zprime1700A700','Zprime2000A500','Zprime2500A300','Zprime2500A700','Zprime800A300','Zprime1000A600','Zprime1200A400','Zprime1200A800','Zprime1400A600','Zprime1700A400','Zprime1700A800','Zprime2000A600','Zprime2500A400','Zprime2500A800','Zprime800A400']
for d in dirs:
        cmd = "combineTool.py -M CollectLimits "+d+ "/*/*/*.limit.* -o "+d+".json"
        os.system(cmd)

