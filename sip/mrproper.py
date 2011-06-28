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
