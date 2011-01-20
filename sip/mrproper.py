############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2011
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################
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
