#!/usr/bin/env python

import os
import os.path
if os.access('./Makefile',os.R_OK) :
    os.system('make clean')
dont_rm_files = ['lima.sip','limaconfig.py.in','configure.py','clean.py','LimaConvertor.h']

for root,dirs,files in os.walk('.') :
    for file_name in files :
        if file_name not in dont_rm_files :
            os.remove(os.path.join(root,file_name))
    break
