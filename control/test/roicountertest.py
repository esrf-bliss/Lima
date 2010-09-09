from Lima.Core import *



myroi=Roi(0,0,100,100)
my_opt_ext=SoftOpExternalMgr()

roictmgr=my_opt_ext.addOp(ROICOUNTERS,"titi",0)


print "ROI before set ", myroi
roictmgr.set([myroi])


print "ROI from roictmgr.get() ", roictmgr.get()
roi_check=roictmgr.get()
print "roi_check=%s" % roi_check
