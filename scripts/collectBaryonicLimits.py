import os
dirs=['Baryonic10000A1','Baryonic10000A1000','Baryonic10000A50','Baryonic1000A1','Baryonic1000A150','Baryonic10A10','Baryonic10A150','Baryonic10A500','Baryonic500A500','Baryonic10000A10','Baryonic10000A150','Baryonic10000A500','Baryonic1000A1000','Baryonic10A1','Baryonic10A1000','Baryonic10A50','Baryonic500A150']
for d in dirs:
        cmd = "combineTool.py -M CollectLimits "+d+ "/*/*/*.limit.* -o "+d+".json"
        os.system(cmd)

