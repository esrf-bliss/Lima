#!/usr/bin/env python

import os
import os.path
import glob
import shutil

from configure import modules
f = file('.gitignore')
patterns = [x.strip() for x in f]


rmList = []

for pat in patterns:
    if pat.find('*') > -1:
        rmList.extend(glob.glob(os.path.join('*',pat)))
    rmList.extend(glob.glob(pat))

    
for filename in rmList :
    if os.path.isdir(filename):
        print "exec: rmtree", filename
        shutil.rmtree(filename)
    else:
        print "exec: rm ", filename
        os.remove(filename)
