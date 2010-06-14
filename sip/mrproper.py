#!/usr/bin/env python

import os
import os.path
import glob
import shutil

modules = ['core', 'simulator', 'espia', 'frelon', 'maxipix']

p = os.popen('svn propget svn:ignore . 2> /dev/null')
patterns = p.readlines()
if not p.close():
    for pat in patterns:
        for f in glob.glob(pat.strip()):
            if os.path.isdir(f):
                print "execture: rmtree", f
                #shutil.rmtree(f)
            else:
                print "execute: rm ", f
                #os.remove(f)
            

for mod in modules:
    try:
        os.chdir(mod)
    except OSError:
        continue
    
    if os.access('./Makefile',os.R_OK) :
        os.system('make clean')
#    dont_rm_files = ['lima.sip','limaconfig.py.in','configure.py','clean.py',
#                     'LimaConvertor.h','lima_init_numpy.cpp', 'Makefile']

    p = os.popen('svn propget svn:ignore . 2> /dev/null')
    patterns = p.readlines()
    if not p.close():
        for pat in patterns:
            for f in glob.glob(pat.strip()):
                print "execute: rm ", f
#            os.remove(f)
            
#    for root,dirs,files in os.walk('.') :
#        for file_name in files :
#            if file_name not in dont_rm_files :
#                os.remove(os.path.join(root,file_name))
#        break

    
    os.chdir('..')
