#!/usr/bin/env python

import os
import os.path
import glob
import shutil

from configure import modules

p = os.popen('svn propget svn:ignore . 2> /dev/null')
patterns = p.readlines()
if not p.close():
    for pat in patterns:
        for f in glob.glob(pat.strip()):
            if os.path.isdir(f):
                print "exec: rmtree", f
                shutil.rmtree(f)
            else:
                print "exec: rm ", f
                os.remove(f)
            

for mod, dirs in modules:
    try:
        os.chdir(mod)
    except OSError:
        continue
    
    if os.access('./Makefile',os.R_OK) :
        os.system('make clean')

    p = os.popen('svn propget svn:ignore . 2> /dev/null')
    patterns = p.readlines()
    if not p.close():
        for pat in patterns:
            for f in glob.glob(pat.strip()):
                print "execute: rm ", f
                os.remove(f)
            
    os.chdir('..')
